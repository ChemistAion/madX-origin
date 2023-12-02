#include "_meta.hpp"

#include "scene.hpp"

namespace madX::Meta::glTF
{
	Scenes GetScenes(const simdjson::dom::element& element)
	{
		Scenes scenes;

		const auto& items = JSON::Get<simdjson::dom::array>(element);
		scenes.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = JSON::Get<simdjson::dom::object>(item);
			auto& scene = scenes.emplace_back();

			for (const auto [key, value] : object)
			{
				if (key == "nodes")
				{
					const auto array = JSON::Get<simdjson::dom::array>(value);

					for (const auto item : array)
					{
						scene.nodes_.push_back(JSON::Get<Index>(item));
					}
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					scene.name_ = JSON::Get<std::string_view>(value);
					continue;
				}
				
				if (key == "extensions")
				{
					continue;
				}

				if (key == "extras")
				{
					scene.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		}

		return scenes;
	}
}
