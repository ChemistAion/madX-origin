#include "pool_dynamic.hpp"

import madX.Backend.Processor;

namespace madX::Backend
{
	//TODO: alling up
	PoolDynamic::PoolDynamic(Processor* processor, std::uint32_t size)
		: ring_ { size }
	{
		HRESULT result;
	
		const auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const auto buffer = CD3DX12_RESOURCE_DESC::Buffer(size);

		auto device = processor->GetDevice();
	
		result = device->CreateCommittedResource
		(
			&heap,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
		);
	
		ThrowIfFailed(result, "PoolDynamic::CreateCommittedResource failed");
		//SetObjectName(resource_.Get(), std::string("PoolDynamic"));

		result = resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
		ThrowIfFailed(result, "PoolDynamic::Map failed");
	}

	////////////////////////////////////////////////////////////////////////////////

	void PoolDynamic::BeginFrame(Frame frame)
	{
		auto index = frame % cEngine_BackBufferCount;

		bb_size_[index] = size_;

		auto to_free = bb_size_[(frame + 1) % cEngine_BackBufferCount];
		
		if (false == ring_.Free(to_free))
		{
			throw std::runtime_error("[PoolDynamic::BeginFrame] Ring::Free failed");
		}

		size_ = 0;
	}

	//TODO: profile me
	Allocation PoolDynamic::Allocate(std::uint32_t size)
    {
		Allocation allocation = {};

		size = Core::AlignUp(size, D3D12_MEMORY_ALIGNMENT);

		if (auto padding = ring_.Padding(size); padding != 0) [[unlikely]]
		{
			size_ += padding;

			if (std::uint32_t offset; false == ring_.Allocate(padding, offset)) [[unlikely]]
			{
				//TODO: consider simple no memory
				throw std::runtime_error("[PoolDynamic::BeginFrame] Ring::Allocate crossover failed");
				return allocation;
			}
		}
		
		if (std::uint32_t offset; ring_.Allocate(size, offset)) [[likely]]
		{
			size_ += size;

			allocation.cpu_ = (D3D12_CPU_VIRTUAL_ADDRESS)(data_ + offset);
			allocation.gpu_= (D3D12_GPU_VIRTUAL_ADDRESS)(resource_->GetGPUVirtualAddress() + offset);
		}

		return allocation;
    }
}

