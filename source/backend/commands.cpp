#include "commands.hpp"

import madX.Backend.Processor;

namespace madX::Backend
{
	Commands::Commands
	(
		Processor* processor,
		const std::uint32_t allocators_count,
		const std::uint32_t lists_count,
		const D3D12_COMMAND_QUEUE_DESC& desc
	)
		: lists_count_ { lists_count } //TODO: vector me
    {
		HRESULT result;
		
		auto device = processor->GetDevice();
		
		pool_.resize(allocators_count);

		for (auto buffer = pool_.begin(); buffer != pool_.end(); ++buffer)
		{
			result = device->CreateCommandAllocator
			(
				desc.Type,
				IID_PPV_ARGS(&buffer->allocator_)
			);
			ThrowIfFailed(result, "Commands::Commands at CreateCommandAllocator failed");
			
			//SetObjectName(buffer->allocator_, std::string("CommandAllocator") + std::to_string(std::distance(pool_.begin(), buffer)));

			buffer->lists_ = new ID3D12GraphicsCommandList2* [lists_count];
		
			for (std::uint32_t index = 0; index < lists_count; ++index)
			{
				result = device->CreateCommandList
				(
					processor->GetNode(),
					desc.Type,
					buffer->allocator_,
					nullptr,
					IID_PPV_ARGS(&buffer->lists_[index])
				);			
				ThrowIfFailed(result, "Commands::Commands at CreateCommandList failed");
				
				//SetObjectName(buffer->lists_[index], std::string("CommandList") + std::to_string(std::distance(pool_.begin(), buffer)) + ":" + std::to_string(index));
		
				result = buffer->lists_[index]->Close();
				ThrowIfFailed(result, "Commands::Commands at ID3D12GraphicsCommandList2::Close failed");
			}
		}
		
		////////////////////////////////////////////////////////////////////////////////
		
		auto queue = desc.Type == D3D12_COMMAND_LIST_TYPE_DIRECT
			? processor->GetGraphicsQueue()
			: processor->GetComputeQueue();
		
		for (auto& buffer : pool_)
		{		
			queue->ExecuteCommandLists
			(
				lists_count,
				(ID3D12CommandList* const*)(buffer.lists_)
			);
		}
		
		//TODO: check it... where it should happen
		processor->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		processor->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);
		
		//TODO: check-it HARD_ERROR
		current_ = &pool_[0];
    }

	Commands::~Commands()
	{
		for (auto& buffer : pool_)
		{
			for (std::uint32_t index = 0; index < lists_count_; ++index)
			{
				buffer.lists_[index]->Release();
			}

			delete [] buffer.lists_;

			buffer.allocator_->Release();
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	ID3D12GraphicsCommandList2* Commands::GetNewCommandList()
	{
		assert(current_ != nullptr);
		assert(current_->used_ < lists_count_);

		ID3D12GraphicsCommandList2* list = current_->lists_[current_->used_++];

		auto result = list->Reset(current_->allocator_, nullptr);
		ThrowIfFailed(result, "Commands::GetNewCommandList at ID3D12GraphicsCommandList2::Reset failed");
		
		//return last_ = list;
		return list;
	}

	void Commands::BeginFrame(Frame frame)
	{
		current_ = &pool_[(frame + 1) % pool_.size()];
		
		auto result = current_->allocator_->Reset();
		ThrowIfFailed(result, "Commands::BeginFrame at ID3D12CommandAllocator::Reset failed");
		
		current_->used_ = 0;
	}
}