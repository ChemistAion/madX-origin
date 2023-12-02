import <cassert>;

import stl.core;
import microsoft.windows;
import microsoft.directx;
import madX.Core.Synchron;
import madX.Backend.Settings;
import madX.Backend.Exception;

using namespace Microsoft::WRL;

import <winerror.h>;
import <combaseapi.h>;

namespace madX::Backend
{
	class Processor;

	class HeapUpload final
	{
		Core::Synchron allocate_;
		Core::Synchron flush_;

		std::byte* begin_ = nullptr;
		std::byte* end_ = nullptr;
		std::byte* current_ = nullptr;

		std::uint64_t fence_value_ = 0;

		ComPtr<ID3D12Resource> resource_;

		ComPtr<ID3D12CommandAllocator> cmd_allocator_;
		ComPtr<ID3D12GraphicsCommandList> cmd_list_;

		ComPtr<ID3D12Fence> fence_;
		HANDLE event_;// = INVALID_HANDLE_VALUE;

		std::mutex lock_;
	public:
		explicit HeapUpload(Processor* processor, std::size_t size);
		
		std::byte* Allocate(std::size_t size, std::uint64_t alignment);

		std::byte* GetBasePointer() const noexcept
		{
			return begin_;
		}

		ID3D12GraphicsCommandList* GetCommandList()
		{
			return cmd_list_.Get();
		}

		ID3D12Resource* GetResource()
		{
			return resource_.Get();
		}

		void Flush(Processor*);
	};
}