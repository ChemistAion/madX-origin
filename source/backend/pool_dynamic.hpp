import stl.core;
import microsoft.windows;
import microsoft.directx;
import madX.Core.Container.Ring;
import madX.Backend.Settings;
import madX.Backend.Extensions;
import madX.Backend.Exception;
import madX.Backend.Frame;

using namespace Microsoft::WRL;

namespace madX::Backend
{
	class Processor;

	class PoolDynamic final
	{
		ComPtr<ID3D12Resource> resource_;
		std::byte* data_ = nullptr;

		Core::Ring<std::uint32_t> ring_;
		std::uint32_t size_ = 0;
		std::uint32_t bb_size_[cEngine_BackBufferCount] = { 0 };
	public:
		explicit PoolDynamic(Processor* processor, std::uint32_t size);

//		~PoolDynamic()
//		{
//			//unmap? release?
//			ring_.Free(ring_.GetSize());
//		}

		////////////////////////////////////////////////////////////////////////////////

		void BeginFrame(Frame frame);

		Allocation Allocate(std::uint32_t size);
	};
}