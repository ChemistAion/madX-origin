#include "factory.hpp"

namespace madX::Backend
{
	std::vector<Adapter> Factory::EnumerateAdapters()
	{
		auto logger = Core::Logger::GetInstance();
		logger->Info("Enumerating GPU hardware");

		HRESULT result;

		////////////////////////////////////////////////////////////////////////////////

		ComPtr<IDXGIFactory2> factory;

//		result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(factory.ReleaseAndGetAddressOf()));
//		ThrowIfFailed(result, "CreateDXGIFactory2 on IDXGIFactory4 interface failed");

		result = CreateDXGIFactory1(IID_PPV_ARGS(factory.ReleaseAndGetAddressOf()));
		ThrowIfFailed(result, "CreateDXGIFactory1 on IDXGIFactory4 interface failed");

		////////////////////////////////////////////////////////////////////////////////

		std::vector<Adapter> adapters;

		ComPtr<IDXGIAdapter1> basic_adapter;

		for (UINT adapter_index = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapter_index, basic_adapter.ReleaseAndGetAddressOf()); ++adapter_index)
		{
			DXGI_ADAPTER_DESC1 desc;

			result = basic_adapter->GetDesc1(&desc);

			if (FAILED(result))
			{
				logger->Error("IDXGIAdapter1::GetDesc1 failed on EnumAdapters1::Index: " + std::to_string(adapter_index) + ", omitted), errno=" + std::to_string(result));
				continue;
			}

			Adapter adapter;

			const auto adapter_wname = std::wstring(desc.Description);
			const auto adapter_name = std::string(adapter_wname.cbegin(), adapter_wname.cend());

			if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0)
			{
				logger->Info("Adapter is software based, omitted:  " + adapter_name);
				continue;
			}

			////////////////////////////////////////////////////////////////////////////////

			auto adapters_size = adapters.size();

			for (const auto& level : cFactory_AllowedDeviceFeaturesLevel)
			{
				auto result = D3D12CreateDevice
				(
					basic_adapter.Get(),
					level.first,
					__uuidof(ID3D12Device), //TODO: proper ID3D12Device level
					nullptr
				);

				if (FAILED(result))
				{
					logger->Error("Adapter: " + adapter_name + " cannot create ID3D12Device, asked for feature level: " + level.second);
					continue;
				}

//				ComPtr<IDXGIAdapter4> adapter;
//				
//				if (FAILED(basic_adapter.As(&adapter)))
//				{
//					logger.Error("Adapter: " + adapter_name + " cannot be upgraded to IDXGIAdapter4, asked for feature level: " + level.second);
//					continue;
//				}

				logger->Info("Adapter: " + adapter_name);
				logger->Info("Feature level: " + level.second);
				logger->Info("Total memory: " + std::to_string(desc.DedicatedVideoMemory >> 20u) + "MB"); //TODO

				adapter.adapter_ = basic_adapter.Detach();
				adapter.memory_ = desc.DedicatedVideoMemory;
				adapter.name_ = adapter_name;
				adapter.level_ = level.first;
				adapter.node_ = 0; //TODO

				adapters.push_back(std::move(adapter));
				break;
			}

			if (adapters.size() == adapters_size)
			{
				logger->Warning("Adapter: " + adapter_name + " was ommited after all feature level tryouts");
			}
		}

		return adapters;
	}

}