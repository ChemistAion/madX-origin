#include "buffer.hpp"

import madX.Backend.Processor;

namespace madX::Backend
{
	//TODO: resource usage... needed?
	//Buffer::Buffer(Processor* processor, const BufferDesc& desc, const BufferData* data)
	Buffer::Buffer(Processor* processor, const BufferDesc& desc)
	    : count_ { desc.count_ }
		, stride_ { desc.stride_ }
		, usage_ { desc.usage_ }
		, bind_ { desc.bind_ }
	    , name_ { desc.name_}
	{
		//assert(size_ != 0);
		assert(count_ != 0);
		assert(stride_ != 0);

		assert(usage_ != ResourceUsage::Default);
		assert(usage_ != ResourceUsage::Staging);

		auto size = size_ = desc.count_ * desc.stride_;

		if (desc.usage_ == ResourceUsage::Static)
		{
			assert(bind_ != ResourceBind::ShaderResource);

			if (desc.data_ == nullptr)
			{
				ThrowIfFailed(E_INVALIDARG, name_ + " as StaticBuffer is not initialized (no data provided)");
			}

			//TODO: check allocation result
			allocation_ = processor->AllocateStaticBuffer(size_);
			std::memcpy(allocation_.cpu_, desc.data_, size);

			assert(size_ == size);

			return;
		}

		if (desc.usage_ == ResourceUsage::Dynamic)
		{
			//assert(bind_ != ResourceBind::ShaderResource);
			//assert(bind_ != ResourceBind::UnorderedAccess);

			if (desc.data_ != nullptr)
			{	
				ThrowIfFailed(E_INVALIDARG, name_ + " as DynamicBuffer must be initialized by mapping");
			}

			//TODO: check allocation result
			allocation_ = processor->AllocateDynamicBuffer(size_);

			SetResourceState(D3D12_RESOURCE_STATE_GENERIC_READ);

			return;
		}

		assert(false);
		ThrowIfFailed(E_INVALIDARG, name_ + " unsupported ResourceUsage");
	}
};

//	std::uint32_t align_mask = 1;
//
//	//TODO: staging
//	//TODO: vs D3D12_TEXTURE_DATA_PITCH_ALIGNMENT
//	if (align_mask != 1)
//	{
//		size_ = (desc.size_ + align_mask) & (~align_mask);
//	}

//	if (desc.bind_ == ResourceBind::UniformBuffer)
//	{
//		assert(desc.size_ < D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
//		desc_.size_ = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
//	}