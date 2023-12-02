import <cassert>;

import stl.core;
import microsoft.windows;
import microsoft.directx;
import madX.Backend.Exception;
import madX.Backend.Frame;

using namespace Microsoft::WRL;

import <winerror.h>;
import <combaseapi.h>;

namespace madX::Backend
{
	class Processor;

	class Commands final
	{
		struct CommandsBuffer
		{
			ID3D12CommandAllocator* allocator_ = nullptr;
			ID3D12GraphicsCommandList2** lists_ = nullptr;
			std::uint32_t used_ = 0;
		};

		std::vector<CommandsBuffer> pool_;
		CommandsBuffer* current_ = nullptr;

		//ID3D12GraphicsCommandList2* last_ = nullptr;

		const std::uint32_t lists_count_ = 0;
	public:
		explicit Commands(Processor*, const std::uint32_t, const std::uint32_t, const D3D12_COMMAND_QUEUE_DESC&);

		Commands() = default;
		~Commands();

		Commands(const Commands&) = delete;
		Commands& operator=(const Commands&) = delete;

		Commands(Commands&&) = delete;
		Commands& operator=(Commands&&) = delete;

		////////////////////////////////////////////////////////////////////////////////

	//	ID3D12CommandAllocator* GetAllocator() noexcept
	//	{
	//		assert(current_);
	//		return current_->allocator_;
	//	}

		ID3D12GraphicsCommandList2* GetCurrentCommandList() noexcept
		{
			assert(current_ != nullptr);
			assert(current_->used_ > 0);
			return current_->lists_[current_->used_ - 1];
		}

		ID3D12GraphicsCommandList2* GetNewCommandList();

		//	ID3D12GraphicsCommandList2* GetLastCommandList()
		//	{
		//		assert(last_);
		//		return last_;
		//	}

		void BeginFrame(Frame frame);
	};
}