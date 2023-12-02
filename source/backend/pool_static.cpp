#include "pool_static.hpp"

import madX.Backend.Processor;

namespace madX::Backend
{
	PoolStatic::PoolStatic(Processor* processor, std::uint32_t size)
		: size_ { size }
	{
		HRESULT result;

		auto device = processor->GetDevice();

		//const auto buffer = CD3DX12_RESOURCE_DESC::Buffer(size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		const auto buffer = CD3DX12_RESOURCE_DESC::Buffer(size);

		const auto heap_default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		const auto heap_upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

		result = device->CreateCommittedResource
		(
			&heap_default,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			nullptr,
			IID_PPV_ARGS(mem_gpu_.ReleaseAndGetAddressOf())
		);

		ThrowIfFailed(result, "PoolStatic::CreateCommittedResource (video mem) failed");
		
		//result = mem_gpu_.Get()->SetName(L"PoolStatic::Video");
		//ThrowIfFailed(result, "PoolStatic::Video::SetName failed");

		////////////////////////////////////////////////////////////////////////////////

		result = device->CreateCommittedResource
		(
			&heap_upload,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(mem_cpu_.ReleaseAndGetAddressOf())
		);

		ThrowIfFailed(result, "PoolStatic::CreateCommittedResource (system mem) failed");

		//result = mem_gpu_.Get()->SetName(L"PoolStatic::System");
		//ThrowIfFailed(result, "PoolStatic::System::SetName failed");

		result = mem_cpu_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
		ThrowIfFailed(result, "PoolStatic::Map (system mem) failed");
	}

	Allocation PoolStatic::Allocate(std::uint32_t size)
	{
		Allocation allocation = {};
	
		size = Core::AlignUp(size, D3D12_MEMORY_ALIGNMENT);
		assert(offset_ + size < size_);

		allocation.cpu_= (D3D12_CPU_VIRTUAL_ADDRESS)(data_ + offset_);
		allocation.gpu_= (D3D12_GPU_VIRTUAL_ADDRESS)(mem_gpu_->GetGPUVirtualAddress() + offset_);

		offset_ += size;
	
		return allocation;
	}

	void PoolStatic::UploadData(ID3D12GraphicsCommandList* command_list)
	{
		//TODO: improve me...
		assert(command_list);

		D3D12_RESOURCE_BARRIER barrier;

		barrier = CD3DX12_RESOURCE_BARRIER::Transition(mem_gpu_.Get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);
		command_list->ResourceBarrier(1, &barrier);

		command_list->CopyBufferRegion
		(
			mem_gpu_.Get(),
			init_,
			mem_cpu_.Get(),
			init_,
			offset_ - init_
		);

		barrier = CD3DX12_RESOURCE_BARRIER::Transition(mem_gpu_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		command_list->ResourceBarrier(1, &barrier);

		init_ = offset_;
	}
}