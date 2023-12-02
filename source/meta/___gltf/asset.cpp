#include "_meta.hpp"

#include "asset.hpp"

namespace madX::Meta::glTF
{
	Asset GetAsset(const simdjson::dom::element& element)
	{
		Asset asset;

		for (const auto [key, value] : JSON::Get<simdjson::dom::object>(element))
		{
			if (key == "copyright")
			{
				asset.copyright_ = JSON::Get<std::string_view>(value);
				continue;
			}
		
			if (key == "generator")
			{
				asset.generator_ = JSON::Get<std::string_view>(value);
				continue;
			}
			
			if (key == "version")
			{
				asset.version_ = JSON::Get<std::string_view>(value);
				continue;
			}
			
			if (key == "minVersion")
			{
				asset.compatibility_ = JSON::Get<std::string_view>(value);
				continue;
			}
			
			if (key == "extensions")
			{
				continue;
			}
			
			if (key == "extras")
			{
				asset.extras_ = std::move(GetExtras(value));
				continue;
			}
			
			assert(false);
		}

		return asset;
	}
}
