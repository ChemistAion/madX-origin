#include "heap_descriptor.hpp"

import madX.Backend.Processor;

namespace madX::Backend
{
	HeapDescriptor::HeapDescriptor
	(
		Processor* processor,
		std::uint32_t count,
		D3D12_DESCRIPTOR_HEAP_TYPE type
	)
		: count_ { count }
		//additional heap for UAVs with GPU shader-visible and CPU non-shader-visible
	    , size_ { processor->GetDevice()->GetDescriptorHandleIncrementSize(type != D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES ? type : D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) }
		, flags_ { ((type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) || (type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)) ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE }
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Type = type != D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES ? type : D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = count;
		desc.Flags = flags_;
		desc.NodeMask = processor->GetNode();

		auto device = processor->GetDevice();
		auto result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(heap_.ReleaseAndGetAddressOf()));

		//TODO: !!!throw...
		assert(SUCCEEDED(result));

		//TODO: !!!SetName
		//result = mem_gpu_.Get()->SetName(L"PoolStatic::Video");
		//ThrowIfFailed(result, "PoolStatic::Video::SetName failed");
	}

	HeapDescriptor::Element HeapDescriptor::Allocate(std::uint32_t count)
    {
		assert(heap_);

		Element element;

		if ((index_ + count) > count_)
		{
			assert(false);
			return element;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle_cpu = heap_->GetCPUDescriptorHandleForHeapStart();
		handle_cpu.ptr += index_ * size_;

		if (flags_ == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE handle_gpu = heap_->GetGPUDescriptorHandleForHeapStart();
			handle_gpu.ptr += index_ * size_;

			element.gpu_ = handle_gpu;
		}

		index_ += count;

		element.cpu_ = handle_cpu;

		element.count_ = count;
		element.size_ = size_;
		element.heap_ = heap_.Get();

		return element;
    }
}