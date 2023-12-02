export module madX.Backend.Settings;

import stl.core;
import microsoft.windows;

export namespace madX::Backend
{
	constexpr bool cEngineMode_Experimental = false;

	constexpr bool cEngineMode_Debug = true;
	constexpr bool cEngineMode_DebugGPV = false;
	constexpr bool cEngineMode_DebugSCQV = false;

	constexpr bool cEngineMode_PIX = false;
	constexpr bool cEngineMode_Aftermatch = false;

	constexpr UINT cEngine_BackBufferCount = 3;
	constexpr UINT cEngine_CommandListPerFrame = 8;

	////////////////////////////////////////////////////////////////////////////////

	constexpr std::size_t cContext_FlushCommandsCount = 256;
	constexpr std::size_t cContext_MaxSlotsCount = 32;

	////////////////////////////////////////////////////////////////////////////////

	constexpr std::uint32_t cHeapDescriptor_GenericSize = 0x3000;
	constexpr std::uint32_t cHeapDescriptor_SamplerSize = 0x10;
	constexpr std::uint32_t cHeapDescriptor_RTVSize = 0x20;
	constexpr std::uint32_t cHeapDescriptor_DSVSize = 0x20;
	constexpr std::uint32_t cHeapDescriptor_UAV_NSV_Size = 0x100;

	////////////////////////////////////////////////////////////////////////////////

	constexpr std::uint32_t cHeapUpload_Size = 0x40000000;
	constexpr std::uint32_t cPoolDynamic_MemorySize = 1u << 24u;
	constexpr std::uint32_t cPoolStatic_MemorySize = 1u << 26u;
}