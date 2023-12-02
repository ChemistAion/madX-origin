export module madX.Backend.Extensions;

import stl.core;
import microsoft.directx;

export namespace madX::Backend
{
	constexpr D3D12_RESOURCE_STATES D3D12_RESOURCE_STATE_INVALID = (D3D12_RESOURCE_STATES)-1;
	typedef void* D3D12_CPU_VIRTUAL_ADDRESS;

	constexpr std::size_t D3D12_MEMORY_ALIGNMENT = 256;

	//Allocation = std::pair<D3D12_CPU_VIRTUAL_ADDRESS, D3D12_GPU_VIRTUAL_ADDRESS>;
	struct Allocation
	{
		D3D12_CPU_VIRTUAL_ADDRESS cpu_ = nullptr;
		D3D12_GPU_VIRTUAL_ADDRESS gpu_ = 0;
	};
}