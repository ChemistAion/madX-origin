import <cassert>;

import stl.core;
import microsoft.windows;
import microsoft.directx;
import madX.Backend.Exception;
import madX.Backend.Frame;

using namespace Microsoft::WRL;

import <winerror.h>;
import <combaseapi.h>;

//#include <handleapi.h>;
//#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

namespace madX::Backend
{
	typedef UINT64 Frame;

	class Processor;

	class Fence final
	{
		HANDLE event_;// { INVALID_HANDLE_VALUE };
		Frame frame_ { 0 };
		ComPtr<ID3D12Fence> fence_;
	public:
		Fence() = default;

		~Fence()
		{
			//if (event_ != INVALID_HANDLE_VALUE)
			{
				::CloseHandle(event_);
			}
		}

		//TODO: check usage (hardfix: swapchain only)
		explicit Fence(class Processor* processor);

		////////////////////////////////////////////////////////////////////////////////

		void Signal(ID3D12CommandQueue* command_queue, Frame frame);
		bool WaitCPU(Frame old_frame);
		void WaitGPU(ID3D12CommandQueue* command_queue);
	};
}