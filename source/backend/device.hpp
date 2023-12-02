import <cassert>;

import stl.core;
import microsoft.windows;
import microsoft.directx;
import madX.Core.Logger;
import madX.Backend.Settings;
import madX.Backend.Exception;

using namespace Microsoft::WRL;

import <winerror.h>;
import <combaseapi.h>;

namespace madX::Backend
{
	class Device
	{
		//Adapter adapter_;
		ComPtr<ID3D12Device> device_;
		ComPtr<ID3D12CommandQueue> queue_direct_;
		ComPtr<ID3D12CommandQueue> queue_compute_;

		//GpuCrashTracker crasher_;

		ComPtr<IDXGIAdapter1> GetHardwareAdapter();

	//	template <typename ObjectDescType>
	//	friend class Object;
	//	
	//	friend class Signature;
	//	
	//	auto GetDevice() const
	//	{
	//		return device_.Get();
	//	}
	public:
		//explicit Device(Adapter& adapter);
		Device();

		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;

		////////////////////////////////////////////////////////////////////////////////

		ID3D12Device* GetDevice() noexcept
		{
			return device_.Get();
		}

		//TODO: implement this
		UINT GetNode() const noexcept
		{
			return 0;
			//return adapter_.node_;
		}

		ID3D12CommandQueue* GetGraphicsQueue() noexcept
		{
			return queue_direct_.Get();
		}

		ID3D12CommandQueue* GetComputeQueue() noexcept
		{
			return queue_compute_.Get();
		}

		void Flush(D3D12_COMMAND_LIST_TYPE queue_type);
	};
}