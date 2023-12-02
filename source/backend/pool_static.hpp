import <cassert>;

import stl.core;
import microsoft.windows;
import microsoft.directx;
import madX.Backend.Settings;
import madX.Backend.Extensions;
import madX.Backend.Exception;

using namespace Microsoft::WRL;

import <winerror.h>;
import <combaseapi.h>;

namespace madX::Backend
{
	class Processor;

	class PoolStatic final
	{
		ComPtr<ID3D12Resource> mem_gpu_;
		ComPtr<ID3D12Resource> mem_cpu_;
		std::byte* data_ = nullptr;

		std::uint64_t offset_ = 0;
		std::uint64_t init_ = 0;
		std::uint64_t size_ = 0;
		
	public:
		explicit PoolStatic(Processor* processor, std::uint32_t size);
		
		//	~PoolStatic()
		//	{
		//		//unmap? release?
		//	}

		////////////////////////////////////////////////////////////////////////////////

		//	void BeginFrame(Frame frame);

		Allocation Allocate(std::uint32_t size);

		void UploadData(ID3D12GraphicsCommandList* command_list);

		void Free()
		{
			mem_cpu_.Reset();
		}
	};
}