#include "device.hpp"

namespace madX::Backend
{
	ComPtr<IDXGIAdapter1> Device::GetHardwareAdapter()
	{
		ComPtr<IDXGIFactory4> factory;

		UINT createFactoryFlags = 0;
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

		//TODO: bug "& vs ReleaseAndGetAddressOf"
		ThrowIfFailed
		(
			CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(factory.ReleaseAndGetAddressOf())),
			"Device::GetHardwareAdapter CreateDXGIFactory2 failed"
		);
		//ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)), "Device::GetHardwareAdapter CreateDXGIFactory2 failed");

		ComPtr<IDXGIAdapter1> adapter;
		//ComPtr<IDXGIAdapter4> dxgiAdapter4;

		for (UINT index = 0; factory->EnumAdapters1(index, adapter.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND; ++index)
		{
			HRESULT result;
		
			DXGI_ADAPTER_DESC1 adapter_desc;
			result = adapter->GetDesc1(&adapter_desc);
		
			ThrowIfFailed(result, "Device::GetHardwareAdapter IDXGIAdapter1::GetDesc1 failed");
		
			if (adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}
		
			result = D3D12CreateDevice
			(
				adapter.Get(),
				D3D_FEATURE_LEVEL_12_0,
				__uuidof(ID3D12Device),
				nullptr
			);
		
			if (SUCCEEDED(result))
			{
				return adapter;
			}
		}

		assert(false);
		return nullptr;
	}

	//Device::Device(Adapter& adapter)
	Device::Device()
		//: adapter_{adapter}
	{
		HRESULT result;

		auto logger = Core::Logger::GetInstance();

		////////////////////////////////////////////////////////////////////////////////

	//	if constexpr (cEngineMode_Experimental)
	//	{
	//		logger->Info("D3D12 ExperimentalFeatures...");
	//	
	//		UUID features[] =
	//		{
	//			//D3D12ExperimentalShaderModels
	//		};
	//	
	//		result = D3D12EnableExperimentalFeatures
	//		(
	//			(UINT)_countof(features),
	//			features,
	//			nullptr,
	//			nullptr
	//		);
	//	
	//		if (SUCCEEDED(result))
	//		{
	//			logger->Info("* D3D12ExperimentalShaderModels enabled");
	//		}
	//		else
	//		{
	//			logger->Error("* D3D12ExperimentalShaderModels failed, errno=" + std::to_string(result));
	//		}
	//	}
	//	else
	//	{
	//		logger->Info("D3D12 ExperimentalFeatures disabled");
	//	}

		//logger.Info("Creating DX12 Device on: ");
		//logger.Info("Creating DX12 Device on: " + adapter.name_);

		////////////////////////////////////////////////////////////////////////////////

		if constexpr (cEngineMode_Debug && !cEngineMode_PIX && !cEngineMode_Aftermatch)
		{
			logger->Info("DebugControllers...");

			ComPtr<ID3D12Debug> basic_controller;

			result = D3D12GetDebugInterface(IID_PPV_ARGS(basic_controller.ReleaseAndGetAddressOf()));
		
			if (SUCCEEDED(result))
			{
				//TODO: switch...
				basic_controller->EnableDebugLayer();
			
				logger->Info("* ID3D12Debug enabled");
				
				ComPtr<ID3D12Debug3> ext_controller;
				result = basic_controller.As(&ext_controller);
				
				if (SUCCEEDED(result))
				{
					logger->Info("* ID3D12Debug3 enabled");
				
					ext_controller->SetEnableGPUBasedValidation(cEngineMode_DebugGPV);
					ext_controller->SetEnableSynchronizedCommandQueueValidation(cEngineMode_DebugSCQV);
				}
				else
				{
					logger->Error("* ID3D12Debug3 failed, errno=" + std::to_string(result));
				}
			}
			else
			{
				logger->Error("* ID3D12Debug failed, errno=" + std::to_string(result));
			}
		}
		else
		{
			logger->Info("DebugControllers disabled");
		}

		////////////////////////////////////////////////////////////////////////////////

		auto adapter = GetHardwareAdapter();

		try
		{
			//crasher_.Initialize();

			result = D3D12CreateDevice
			(
				adapter.Get(),
				D3D_FEATURE_LEVEL_12_0,
				IID_PPV_ARGS(device_.ReleaseAndGetAddressOf())
			);

			//TODO: result handling...
		}
		catch (const _com_error& error)
		{
			std::wstring error_message(error.ErrorMessage());
			logger->Critical("D3D12CreateDevice:" + std::string(error_message.cbegin(), error_message.cend()));
		}
		catch (...)
		{
			logger->Critical("D3D12CreateDevice: Unhandled exception");
		}

		//ThrowIfFailed(result, "D3D12CreateDevice failed");

	//	if constexpr (cEngineMode_Aftermatch && !cEngineMode_Debug)
	//	{
	//		logger->Info("NVIDIA Nsight Aftermath crash reporter...");
	//	
	//		const std::uint32_t aftermath_flags =
	//			GFSDK_Aftermath_FeatureFlags_EnableMarkers |
	//			GFSDK_Aftermath_FeatureFlags_EnableResourceTracking |
	//			GFSDK_Aftermath_FeatureFlags_CallStackCapturing;
	//	
	//		result = GFSDK_Aftermath_DX12_Initialize
	//		(
	//			GFSDK_Aftermath_Version_API,
	//			aftermath_flags,
	//			device_.Get()
	//		);
	//	
	//		if (result == GFSDK_Aftermath_Result_Success)
	//		{
	//			logger->Info("* Success");
	//		}
	//		else
	//		{
	//			logger->Error("* Failed, errno=" + std::to_string(result));
	//		}
	//	}

		////////////////////////////////////////////////////////////////////////////////

		if constexpr (cEngineMode_Debug && !cEngineMode_PIX && !cEngineMode_Aftermatch)
		{
			//logger->Info("InfoQueue...");
		
			ComPtr<ID3D12InfoQueue> info_queue;
		
			result = device_->QueryInterface(IID_PPV_ARGS(info_queue.ReleaseAndGetAddressOf()));
		
			if (SUCCEEDED(result))
			{
				logger->Info("* ID3D12InfoQueue enabled");
		
				////////////////////////////////////////////////////////////////////////////////
				
				D3D12_INFO_QUEUE_FILTER filter = {};
		
				//D3D12_MESSAGE_CATEGORY message_categories[] =
				//{
				//};
				//
				//filter.DenyList.NumCategories = _countof(message_categories);
				//filter.DenyList.pSeverityList = message_categories;
		
				D3D12_MESSAGE_SEVERITY message_severities[] =
				{
					D3D12_MESSAGE_SEVERITY_MESSAGE,
					D3D12_MESSAGE_SEVERITY_INFO
				};
		
				filter.DenyList.NumSeverities = _countof(message_severities);
				filter.DenyList.pSeverityList = message_severities;
		
				//D3D12_MESSAGE_ID message_ids[] =
				//{
				//	D3D12_MESSAGE_ID_HEAP_ADDRESS_RANGE_INTERSECTS_MULTIPLE_BUFFERS,
				//	D3D12_MESSAGE_ID_GETHEAPPROPERTIES_INVALIDRESOURCE,
				//	D3D12_MESSAGE_ID_CREATERESOURCE_INVALIDALIGNMENT,
				//	D3D12_MESSAGE_ID_NON_RETAIL_SHADER_MODEL_WONT_VALIDATE,
				//	D3D12_MESSAGE_ID_CREATEPIPELINESTATE_CACHEDBLOBDESCMISMATCH,
				//	D3D12_MESSAGE_ID_EMPTY_DISPATCH
				//};
				//
				//filter.DenyList.NumIDs = _countof(message_ids);
				//filter.DenyList.pIDList = message_ids;		
		
				result = info_queue->AddStorageFilterEntries(&filter);
		
				if (SUCCEEDED(result))
				{
					logger->Info("* ID3D12InfoQueue::AddStorageFilterEntries success");
				}
				else
				{
					logger->Error("* ID3D12InfoQueue::AddStorageFilterEntries failed, errno=" + std::to_string(result));
				}
			
				////////////////////////////////////////////////////////////////////////////////
		
				info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
				info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
				info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, FALSE);
			}
			else
			{
				logger->Error("* ID3D12InfoQueue failed, errno=" + std::to_string(result));
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		//assert(cDevice_CommandQueuesCount == 1);

		{
			D3D12_COMMAND_QUEUE_DESC queue_desc = {};
			queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			queue_desc.NodeMask = GetNode();

			result = device_->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(queue_direct_.ReleaseAndGetAddressOf()));
			ThrowIfFailed(result, "Device::Device ID3D12CommandQueue::DirectQueue failed");
			
			//result = queue_direct_->SetName(L"DirectQueue");
		}

		{
			D3D12_COMMAND_QUEUE_DESC queue_desc = {};
			queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			queue_desc.NodeMask = GetNode();

			result = device_->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(queue_compute_.ReleaseAndGetAddressOf()));
			ThrowIfFailed(result, "Device::Device ID3D12CommandQueue::ComputeQueue failed");
			
			//result = queue_compute_->SetName(L"ComputeQueue");
		}
	}

	void Device::Flush(D3D12_COMMAND_LIST_TYPE queue_type)
	{
		HRESULT result;

		ID3D12Fence* fence;

		result = device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		ThrowIfFailed(result, "Device::Flush CreateFence failed");

		auto queue = queue_type == D3D12_COMMAND_LIST_TYPE_DIRECT
			? GetGraphicsQueue()
			: GetComputeQueue();

		result = queue->Signal(fence, 1);
		ThrowIfFailed(result, "Device::Flush SignalFence failed");

		HANDLE event = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);

		fence->SetEventOnCompletion(1, event);
		::WaitForSingleObject(event, INFINITE);
		::CloseHandle(event);

		fence->Release();
	}
}

//		{
//			D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
//			device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options));
//		
//			D3D12_FEATURE_DATA_D3D12_OPTIONS1 options1 = {};
//			device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS1, &options1, sizeof(options1));
//		
//			D3D12_FEATURE_DATA_D3D12_OPTIONS2 options2 = {};
//			device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS2, &options2, sizeof(options2));
//		
//			D3D12_FEATURE_DATA_D3D12_OPTIONS3 options3 = {};
//			device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS3, &options3, sizeof(options3));
//		
//			D3D12_FEATURE_DATA_D3D12_OPTIONS4 options4 = {};
//			device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS4, &options4, sizeof(options4));
//		
//			D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
//			device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5));
//		
//			D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6 = {};
//			device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &options6, sizeof(options6));
//		
//			D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7 = {};
//			device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options7, sizeof(options7));
//		}