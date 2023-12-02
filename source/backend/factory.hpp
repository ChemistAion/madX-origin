import stl.core;
import microsoft.windows;
import microsoft.directx;
import madX.Core.Logger;
import madX.Core.Singleton;
import madX.Backend.Exception;

using namespace Microsoft::WRL;

import <winerror.h>;
import <combaseapi.h>;

namespace madX::Backend
{
	class Factory;

	std::vector<std::pair<D3D_FEATURE_LEVEL, std::string>> const cFactory_AllowedDeviceFeaturesLevel =
	{
			{	D3D_FEATURE_LEVEL_12_1, "12.1" },
			{	D3D_FEATURE_LEVEL_12_0, "12.0" },
			{	D3D_FEATURE_LEVEL_11_1, "11.1" },
			{	D3D_FEATURE_LEVEL_11_0, "11.0" },
	};

	struct Adapter
	{
		IDXGIAdapter1* adapter_ = nullptr;

		std::string name_;
		std::size_t memory_ = 0;

		D3D_FEATURE_LEVEL level_ = D3D_FEATURE_LEVEL_11_0;
		UINT node_ = 0;
	};

	class Factory final : public Core::Singleton<Factory>
	{
	public:
		////////////////////////////////////////////////////////////////////////////////

		std::vector<Adapter> EnumerateAdapters();
	};
}