import <cassert>;

import stl.core;
import microsoft.windows;
import microsoft.directx;
import madX.Backend.Exception;
import madX.Backend.HeapDescriptor;
import madX.Backend.Shader;

using namespace Microsoft::WRL;

import <winerror.h>;
import <combaseapi.h>;

namespace madX::Backend
{
	struct SignatureAttributes
	{
		const std::size_t index_ = (std::size_t)-1;
		const std::size_t offset_ = (std::size_t)-1;
		const std::size_t count_ = (std::size_t)-1;

		const ShaderInputType type_ = ShaderInputType::Unknown;

		constexpr SignatureAttributes() = default;
		constexpr explicit SignatureAttributes
		(
			std::size_t index,
			std::size_t offset,
			std::size_t count,
			ShaderInputType type
		)
			: index_ { index }
			, offset_{ offset }
			, count_{ count }
			, type_{ type }
		{
		}

		constexpr bool IsValid() const noexcept
		{
			return type_ != ShaderInputType::Unknown;
		}
	};

	////////////////////////////////////////////////////////////////////////////////

	class Processor;
	class Context;

	class Signature final
	{
		ComPtr<ID3D12RootSignature> root_signature_;

		using Parameter = D3D12_ROOT_PARAMETER1;
		std::vector<Parameter> parameters_;

		using Range = std::vector<D3D12_DESCRIPTOR_RANGE1>;
		std::list<Range> ranges_;

		using Sampler = D3D12_STATIC_SAMPLER_DESC;
		std::vector<Sampler> samplers_;

		std::map<std::string, SignatureAttributes> attributes_;
	
		HeapDescriptor::Element heap_generic_;
		HeapDescriptor::Element heap_sampler_;

		Processor* processor_ = nullptr;
	public:
		Signature(Processor* processor)
		  : processor_ { processor }
		{	
		}

		Signature(const Signature&) = delete;
		Signature& operator=(const Signature&) = delete;

		Signature(Signature&&) = delete;
		Signature& operator=(Signature&&) = delete;

		////////////////////////////////////////////////////////////////////////////////

		void AddShaderResources(const std::shared_ptr<Shader>& shader);

		ID3D12RootSignature* GetRootSignature() const noexcept
		{
			assert(root_signature_);
			return root_signature_.Get();
		}

		void Finalize();
		void CommitBindings(Context& context);
	
		////////////////////////////////////////////////////////////////////////////////

		SignatureAttributes GetAttributes(const std::string& name) const noexcept
		{
			if (auto found = attributes_.find(name); found != attributes_.cend())
			{
				return found->second;
			}

			assert(false);
			return {};
		}

	//	//TODO: SFINAE
	//	template<D3D12_DESCRIPTOR_HEAP_TYPE heap_type>
	//	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandleCPU(SignatureAttributes attributes, UINT offset) const
	//	{
	//		//TODO!: fixed offset
	//		assert(offset == 0);
	//		assert(attributes.offset_ == 0);
	//	
	//		//TODO!: fixed - must inlcude proper index etc.
	//		auto index = attributes.index_ + attributes.offset_ + offset;
	//	
	//		if constexpr (heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
	//		{
	//			assert(heap_generic_.IsValid());
	//			return heap_generic_.GetHandleCPU(index);
	//		}
	//	
	//		if constexpr (heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
	//		{
	//			assert(heap_sampler_.IsValid());
	//			return heap_sampler_.GetHandleCPU(index);
	//		}
	//	
	//		assert(false);
	//	}
	//	
	//	//TODO: SFINAE
	//	template<D3D12_DESCRIPTOR_HEAP_TYPE heap_type>
	//	D3D12_GPU_DESCRIPTOR_HANDLE GetDescriptorHandleGPU(SignatureAttributes attributes, UINT offset) const
	//	{
	//		//TODO!: fixed offset
	//		assert(offset == 0);
	//		assert(attributes.offset_ == 0);
	//	
	//		//TODO!: fixed - must inlcude proper index etc.
	//		auto index = attributes.index_ + attributes.offset_ + offset;
	//	
	//		//TODO: type conversion std::size_t <-> std::uint32_t
	//		if constexpr (heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
	//		{
	//			assert(heap_generic_.IsValid());
	//			return heap_generic_.GetHandleGPU(std::uint32_t(index));
	//		}
	//	
	//		if constexpr (heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
	//		{
	//			assert(heap_sampler_.IsValid());
	//			return heap_sampler_.GetHandleGPU(std::uint32_t(index));
	//		}
	//	
	//		assert(false);
	//	}
	};
}