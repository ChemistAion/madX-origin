#include "signature.hpp"

import madX.Backend.Context;
import madX.Backend.Processor;

//https://simoncoenen.com/blog/programming/graphics/DxcCompiling
namespace madX::Backend
{
    void Signature::AddShaderResources(const std::shared_ptr<Shader>& shader)
    {
		std::map<ShaderInputAttributes, std::string> bindings;
		
		{
			HRESULT result;

			auto reflection = shader->GetReflection();

			D3D12_SHADER_DESC shader_desc;
			result = reflection->GetDesc(&shader_desc);
			ThrowIfFailed(result, "[Signature::AddShaderResources] ID3D12ShaderReflection::GetDesc failed");

			for (UINT index = 0; index < shader_desc.BoundResources; ++index)
			{
				D3D12_SHADER_INPUT_BIND_DESC desc;

				result = reflection->GetResourceBindingDesc(index, &desc);
				ThrowIfFailed(result, "[Signature::AddShaderResources] ID3D12ShaderReflection::GetResourceBindingDesc failed");

				auto result = bindings.emplace(desc, desc.Name);
				assert(result.second);
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		const auto& signature = shader->GetSignature();

		for (UINT index = 0; index < signature.NumParameters; ++index)
		{
			const auto& parameter = signature.pParameters[index];
	
			switch (parameter.ParameterType)
			{
			case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
			{
				assert(false);
				break;
			}
			case D3D12_ROOT_PARAMETER_TYPE_CBV:
			case D3D12_ROOT_PARAMETER_TYPE_SRV:
			case D3D12_ROOT_PARAMETER_TYPE_UAV:
			{
				auto found = std::find_if
				(
					bindings.cbegin(),
					bindings.cend(),
					[&parameter](const auto& binding)
					{
						auto type = false;
						
						switch (binding.first.type_)
						{
						case ShaderInputType::CBV:
						{
							type = parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV;
							break;
						}
						case ShaderInputType::BufferSRV:
						case ShaderInputType::TextureSRV:
						{
							type = parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV;
							break;
						}
						case ShaderInputType::BufferUAV:
						case ShaderInputType::TextureUAV:
						{
							type = parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_UAV;
							break;
						}
						case ShaderInputType::Sampler:
						default:
						{
							assert(false);
							break;
						}
						}
						
						auto space = binding.first.space_ == parameter.Descriptor.RegisterSpace;
						auto point = binding.first.point_ == parameter.Descriptor.ShaderRegister;
						auto count = binding.first.count_ == 1;
						
						return type && space && point && count;
					}
				);

				assert(found != bindings.cend());

				auto result = attributes_.emplace
				(
					found->second,
					SignatureAttributes
					(
						parameters_.size(),
						0u,
						0u,
						found->first.type_
					)
				);

				if (result.second)
				{			
					parameters_.emplace_back(parameter);
				}
			//	else
			//	{
			//		assert(false);
			//	}

				break;
			}
			case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
			{
				Range ranges;

				const auto& table = parameter.DescriptorTable;

				ranges.reserve(table.NumDescriptorRanges);

				for (UINT index = 0; index < table.NumDescriptorRanges; ++index)
				{
					const auto& range = table.pDescriptorRanges[index];

					auto found = std::find_if
					(
						bindings.cbegin(),
						bindings.cend(),
						[&range](const auto& binding)
						{
							auto type = false;
							
							switch (binding.first.type_)
							{
							case ShaderInputType::CBV:
							{
								type = range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
								break;
							}
							case ShaderInputType::BufferSRV:
							case ShaderInputType::TextureSRV:
							{
								type = range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
								break;
							}
							case ShaderInputType::BufferUAV:
							case ShaderInputType::TextureUAV:
							{
								type = range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
								break;
							}
							case ShaderInputType::Sampler:
							{
								type = range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
								break;
							}
							default:
							{
								break;
							}
							}
							
							auto space = binding.first.space_ == range.RegisterSpace;
							auto point = binding.first.point_ == range.BaseShaderRegister;
							auto count = binding.first.count_ == range.NumDescriptors;
							
							return type && space && point && count;
						}
					);

					assert(found != bindings.cend());

					auto result = attributes_.emplace
					(
						found->second,
						SignatureAttributes
						(
							parameters_.size(),
							ranges.size(),
							table.NumDescriptorRanges,
							found->first.type_
						)
					);

					if (result.second)
					{
						ranges.emplace_back(range);
					}
					else
					{
						assert(false);
					}
				}
			
				CD3DX12_ROOT_PARAMETER1 root_parameter;
				root_parameter.InitAsDescriptorTable
				(
					(UINT)(ranges.size()),
					ranges.data(),
					parameter.ShaderVisibility
				);
			
				ranges_.emplace_back(std::move(ranges));
				parameters_.emplace_back(std::move(root_parameter));

				break;
			}
			default:
				break;
			}
		}
	
		for (UINT index = 0; index < signature.NumStaticSamplers; ++index)
		{
			const auto& sampler = signature.pStaticSamplers[index];
			samplers_.emplace_back(sampler);
		}
    }

	////////////////////////////////////////////////////////////////////////////////

	//TODO: change to all-commmits at once!
	void Signature::Finalize()
	{
		//TODO: hard-coded D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc
		(
			(UINT)parameters_.size(),
			parameters_.data(),
			(UINT)samplers_.size(),
			samplers_.data(),
		    D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		);
	
		HRESULT result;
	
		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
	
		result = D3D12SerializeVersionedRootSignature
		(
			&desc,
			&signature,
			&error
		);
	
		//TODO: error message handling
		ThrowIfFailed(result, "[Signature::Finalize] Failed to serialize RootSignature");
	
		////////////////////////////////////////////////////////////////////////////////
	
		auto device = processor_->GetDevice();
		result = device->CreateRootSignature
		(
			processor_->GetNode(),
			signature->GetBufferPointer(),
			signature->GetBufferSize(),
			IID_PPV_ARGS(root_signature_.ReleaseAndGetAddressOf())
		);
	
		ThrowIfFailed(result, "[Signature::Finalize] Failed to create RootSignature");

		////////////////////////////////////////////////////////////////////////////////
		//TODO: cache...

		std::uint32_t parameter_count = (std::uint32_t)parameters_.size();
		std::uint32_t sampler_count = (std::uint32_t)samplers_.size();

		//	//TODO: count during AddShaderResources
		//	for (const auto& attribute : attributes_)
		//	{
		//		auto type = attribute.second.type_;
		//		assert(type != ShaderInputType::Unknown);
		//	
		//		//TODO: flags!!! missing
		//	
		//		if (type != ShaderInputType::Sampler)
		//		{
		//			++parameter_count;
		//		}
		//		else
		//		{
		//			++sampler_count;
		//		}
		//	}

		////////////////////////////////////////////////////////////////////////////////

		if (parameter_count != 0)
		{
			heap_generic_ = processor_->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, parameter_count);
			assert(heap_generic_.IsValid());
		}	
		assert((parameter_count == 0 && !heap_generic_.IsValid()) || (parameter_count != 0 && heap_generic_.IsValid()));
		
		if (sampler_count != 0)
		{
			heap_sampler_ = processor_->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, sampler_count);
			assert(heap_sampler_.IsValid());
		}
		assert((sampler_count == 0 && !heap_sampler_.IsValid()) || (sampler_count != 0 && heap_sampler_.IsValid()));
	}

	void Signature::CommitBindings(Context& context)
	{
		DescriptorHeaps heaps
		(
			heap_generic_.GetDescriptorHeap(),
			heap_sampler_.GetDescriptorHeap()
		);
		
		context.SetDescriptorHeaps(heaps);

	//	auto command_list = context.GetCommandList();
	//	assert(command_list != nullptr);
	//	
	//	for (const auto& attribute : attributes_)
	//	{
	//		const auto& signature = attribute.second;
	//		assert(signature.IsValid());
	//		assert(signature.offset_ == 0);
	//	
	//		//TODO: CBV
	//		if (signature.count_ == 0)
	//		{
	//			continue;
	//		}
	//	
	//		bool is_resource = signature.type_ != ShaderInputType::Sampler;
	//	
	//		D3D12_GPU_DESCRIPTOR_HANDLE handle = is_resource
	//		    ? GetDescriptorHandleGPU<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>(signature, 0)
	//		    : GetDescriptorHandleGPU<D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER>(signature, 0);
	//		
	//	
	//		command_list->SetGraphicsRootDescriptorTable((UINT)signature.index_, handle);
	//	}
	}
}

//	void Signature::CommitConstantBuffers(ContextOld* context, CommandContextOld* command_context)
//	{
//		assert(context != nullptr);
//		assert(command_context != nullptr);
//	}

//	void Signature::CommitConstantBuffers
//	(
//		ContextOld* context,
//		CommandContextOld* command_context,
//		ShaderCache* cache
//	)
//	{
//		assert(context != nullptr);
//		assert(command_context != nullptr);
//		assert(cache != nullptr);
//	
//		//		for (const auto& development : development_)
//		//		{
//		//			assert(development.binding_.GetType() != ShaderBindingType::Unknown);
//		//		
//		//			if (development.binding_.GetType() != ShaderBindingType::CBV)
//		//			{
//		//				continue;
//		//			}
//		//		
//		//			assert(development.binding_.IsValid());
//		//		
//		//			auto& resource = cache->GetResource(development.binding_.GetBindingPoint());
//		//		
//		//			auto* buffer = (Buffer*)resource.object_;
//		//			bool is_dynamic = buffer->GetBufferUsage() == ResourceUsage::Dynamic;
//		//		
//		//			auto cbv_address = buffer->GetAddressGPU(context->GetContextID(), context);
//		//			command_context->GetCommandList()->SetGraphicsRootConstantBufferView
//		//			(
//		//				development.attributes_.index_,
//		//				cbv_address
//		//			);
//		//		}
//	}
