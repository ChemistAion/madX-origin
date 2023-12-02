#include "_meta.hpp"

#include "texture.hpp"

namespace madX::Meta::glTF
{
	Textures GetTextures(const simdjson::dom::element& element)
	{
		Textures textures;

		const auto& items = JSON::Get<simdjson::dom::array>(element);
		textures.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = JSON::Get<simdjson::dom::object>(item);
			auto& texture = textures.emplace_back();

			for (const auto [key, value] : object)
			{
				if (key == "sampler")
				{
					texture.sampler_ = std::move(JSON::Get<Index>(value));
					continue;
				}

				if (key == "source")
				{
					texture.source_ = std::move(JSON::Get<Index>(value));
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					texture.name_ = JSON::Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					continue;
				}

				if (key == "extras")
				{
					texture.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		}

		return textures;
	}
}