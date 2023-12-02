#include "heap_upload.hpp"

import madX.Backend.Processor;

namespace madX::Backend
{
	HeapUpload::HeapUpload(Processor* processor, std::size_t size)
	{
		HRESULT result;

		auto device = processor->GetDevice();

		result = device->CreateCommandAllocator
		(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(cmd_allocator_.ReleaseAndGetAddressOf())
		);

		ThrowIfFailed(result,"HeapUpload::HeapUpload failed on CreateCommandAllocator");
	
		result = device->CreateCommandList
		(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			cmd_allocator_.Get(),
			nullptr,
			IID_PPV_ARGS(cmd_list_.ReleaseAndGetAddressOf())
		);	

		ThrowIfFailed(result, "HeapUpload::HeapUpload failed on CreateCommandList");
		//SetObjectName(cmd_list_.Get(), std::string("HeapUploadCL"));

		////////////////////////////////////////////////////////////////////////////////

		//TODO: node check
		auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto buffer = CD3DX12_RESOURCE_DESC::Buffer(size);

		result = device->CreateCommittedResource
		(
			&heap,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
		);

		ThrowIfFailed(result, "HeapUpload::HeapUpload failed on CreateCommittedResource");
		//SetObjectName(cmd_list_.Get(), std::string("HeapUploadR"));

		////////////////////////////////////////////////////////////////////////////////

		result = resource_->Map(0, nullptr, (void**)&begin_);
		ThrowIfFailed(result, "PoolDynamic::Map failed");

		current_ = begin_;
		end_ = begin_ + buffer.Width;
	}

	std::byte* HeapUpload::Allocate(std::size_t size, std::uint64_t alignment)
	{
		flush_.Wait();

		std::byte* result = nullptr;

		{
			std::unique_lock<std::mutex> lock(lock_);

			assert(size < (size_t)(end_ - begin_));

			current_ = reinterpret_cast<std::byte*>(Core::AlignUp(reinterpret_cast<std::size_t>(current_), std::size_t(alignment)));

			if (current_ >= end_ || current_ + size >= end_)
			{
				return nullptr;
			}

			result = current_;
			current_ += size;
		}

		return result;
	}

	void HeapUpload::Flush(Processor* processor)
	{
		assert(processor != nullptr);

		//TODO: optimize flush usage
		flush_.Wait();
		flush_.Inc();
		allocate_.Wait();

		std::unique_lock<std::mutex> lock(lock_);

		//TODO: stuff...

		ThrowIfFailed(cmd_list_->Close(), "HeapUpload::FlushAndFinish failed on ID3D12GraphicsCommandList::Close");

		//TODO: context
		processor->GetGraphicsQueue()->ExecuteCommandLists(1, CommandListCast(cmd_list_.GetAddressOf()));

		processor->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		processor->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);

		cmd_allocator_->Reset();
		cmd_list_->Reset(cmd_allocator_.Get(), nullptr);

		current_ = begin_;

		flush_.Dec();
	}
}