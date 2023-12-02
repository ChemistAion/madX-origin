#include "pipeline.hpp"

import madX.Backend.Context;
import madX.Backend.Processor;

namespace madX::Backend
{
//	D3D12_GPU_DESCRIPTOR_HANDLE Pipeline::Bind(Processor* processor, TextureView& texture_view)
//	{
//		assert(processor != nullptr);
//		auto texture = texture_view.GetTexture();
//		
//		auto attributes = signature_.GetAttributes(texture->GetName());
//		assert(attributes.IsValid());
//	
//		auto cpu_handle = signature_.GetDescriptorHandleCPU<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>(attributes, 0);
//		auto gpu_handle = signature_.GetDescriptorHandleGPU<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>(attributes, 0);
//	
//		auto device = processor->GetDevice();
//		device->CreateShaderResourceView(texture->GetResource(), nullptr, cpu_handle);
//	
//		return gpu_handle;
//	}

	void Pipeline::CommitShaderResources(Context& context)
	{
		auto command_list = context.GetCommandList();
		assert(command_list);

		command_list->SetGraphicsRootSignature(signature_.GetRootSignature());

		signature_.CommitBindings(context);
		//CommitConstantBuffers(context);
	}

	void Pipeline::CommitConstantBuffers(ID3D12GraphicsCommandList* command_list, const std::shared_ptr<Buffer>& buffer)
	{
		assert(command_list);

		auto attributes = signature_.GetAttributes(buffer->GetName());
		assert(attributes.IsValid());

		auto gpu = buffer->GetAddressGPU();
		command_list->SetGraphicsRootConstantBufferView((UINT)attributes.index_, gpu);
	
		//for (const auto& cache : cbv_cache_)
		//{
		//	auto gpu = cache.second->GetAddressGPU(context);
		//	command_list->SetGraphicsRootConstantBufferView((UINT)cache.first, gpu);
		//}
	}

	////////////////////////////////////////////////////////////////////////////////

	Pipeline::Pipeline(Processor* processor, const PipelineDesc& desc)
		: signature_(processor)
	{
		assert(false == desc.name_.empty());
		Core::Logger::GetInstance()->Info("Creating Pipeline: " + desc.name_);

		for (const auto& shader : desc.shaders_)
		{
			if (false == shader.second->HasSignature())
			{
				continue;
			}

			signature_.AddShaderResources(shader.second);
		}
		
		signature_.Finalize();

		////////////////////////////////////////////////////////////////////////////////

		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphics_desc = {};

		graphics_desc.pRootSignature = signature_.GetRootSignature();

		//TODO: transform me...
		for (const auto& shader : desc.shaders_)
		{
			D3D12_SHADER_BYTECODE* code = nullptr;

			switch (shader.first)
			{
			case ShaderType::Vertex:
				code = &graphics_desc.VS;
				break;
			case ShaderType::Pixel:
				code = &graphics_desc.PS;
				break;
			case ShaderType::Geometry:
				code = &graphics_desc.GS;
				break;
			case ShaderType::Hull:
				code = &graphics_desc.HS;
				break;
			case ShaderType::Domain:
				code = &graphics_desc.DS;
				break;
			default:
				assert(false);
				break;
			}

			assert(code != nullptr);

			auto object = shader.second->GetShaderObject();

			assert(object != nullptr);

			code->pShaderBytecode = object->GetBufferPointer();
			code->BytecodeLength = object->GetBufferSize();
		}

		graphics_desc.StreamOutput = {};

		graphics_desc.BlendState = desc.blend_state_;
		graphics_desc.SampleMask = desc.sample_mask_;

		graphics_desc.RasterizerState = desc.rasterizer_state_;
		graphics_desc.DepthStencilState = desc.depth_stencil_state_;

		////////////////////////////////////////////////////////////////////////////////

		graphics_desc.InputLayout.NumElements = (UINT)desc.input_layout_.size();
		graphics_desc.InputLayout.pInputElementDescs = desc.input_layout_.data();	

		////////////////////////////////////////////////////////////////////////////////

		graphics_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		graphics_desc.PrimitiveTopologyType = desc.primitive_topology_;

		graphics_desc.NumRenderTargets = desc.RT_count_;

		//TODO: handle SwapChain RTV
		//TODO: std::vector me
		static_assert(sizeof(graphics_desc.RTVFormats) == sizeof(desc.RTV_format));
		std::memcpy(graphics_desc.RTVFormats, desc.RTV_format, sizeof(DXGI_FORMAT[8]));

		graphics_desc.DSVFormat = desc.DSV_format;
		graphics_desc.SampleDesc = desc.sample_desc_;
		graphics_desc.NodeMask = processor->GetNode();
		graphics_desc.CachedPSO = {};
		graphics_desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		////////////////////////////////////////////////////////////////////////////////

		auto device = processor->GetDevice();
		HRESULT result = device->CreateGraphicsPipelineState
		(
			&graphics_desc,
			IID_PPV_ARGS(state_.ReleaseAndGetAddressOf())
		);

		ThrowIfFailed(result, "Failed to create GraphicsPipelineState");
	}
}