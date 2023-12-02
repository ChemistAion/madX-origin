#include "processor.hpp"

namespace madX::Backend
{
	//	Processor ::Processor (Adapter& adapter)
	Processor ::Processor ()
		: Device()
	    , commands_ { this, cEngine_BackBufferCount, cEngine_CommandListPerFrame, GetGraphicsQueue()->GetDesc() }
		, heaps_
		{
			{ this, cHeapDescriptor_GenericSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV },
			{ this, cHeapDescriptor_SamplerSize, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER },
			{ this, cHeapDescriptor_RTVSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV },
			{ this, cHeapDescriptor_DSVSize, D3D12_DESCRIPTOR_HEAP_TYPE_DSV },
			//additional heap for UAVs with GPU shader-visible and CPU non-shader-visible
			{ this, cHeapDescriptor_UAV_NSV_Size, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES }
		}
		, dynamic_ { this, cPoolDynamic_MemorySize }
		, static_ { this, cPoolStatic_MemorySize }
		, upload_ { this, cHeapUpload_Size }
		, frame_ {}
	{
		return;
	}

	////////////////////////////////////////////////////////////////////////////////

    void Processor ::BeginFrame()
    {
		try
		{
			commands_.BeginFrame(frame_);
			dynamic_.BeginFrame(frame_);
		}
		catch (const _com_error& error)
		{
			auto logger = Core::Logger::GetInstance();
		
			std::wstring error_message(error.ErrorMessage());
			logger->Critical("[Processor ::BeginFrame]: " + std::string(error_message.cbegin(), error_message.cend()));
		}
		catch (const std::runtime_error& error)
		{
			auto logger = Core::Logger::GetInstance();
			logger->Critical("[Processor ::BeginFrame]: " + std::string(error.what()));
		}
		catch (...)
		{
			auto logger = Core::Logger::GetInstance();
			logger->Critical("[Processor ::BeginFrame]: Unhandled exception");
		}
    }
}
