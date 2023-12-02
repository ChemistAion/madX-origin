#include "_meta.hpp"

#include "light.hpp"

namespace madX::Meta::glTF
{
	Lights GetLights(const simdjson::dom::element& element)
	{
		Lights lights;

		const auto& items = JSON::Get<simdjson::dom::array>(element);
		lights.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = JSON::Get<simdjson::dom::object>(item);
			auto& light = lights.emplace_back();

			std::optional<Light::Spot> spot;
			std::optional<std::string> type;
			std::optional<float> range;

			for (const auto [key, value] : object)
			{
				if (key == "color")
				{
					auto vector = JSON::GetVector<Float>(value);
					assert(vector.size() == 3);

					light.color_ = XMFLOAT3(vector.data());
					continue;
				}

				if (key == "intensity")
				{
					light.intensity_ = JSON::Get<Float>(value);
					continue;
				}

				if (key == "type")
				{
					type = JSON::Get<std::string_view>(value);
					continue;
				}

				if (key == "range")
				{
					range = JSON::Get<Float>(value);
					continue;
				}

				if (key == "spot")
				{
					const auto object = JSON::Get<simdjson::dom::object>(item);

					spot = Light::Spot();

					for (const auto [key, value] : object)
					{
						if (key == "innerConeAngle")
						{
							spot->inner_angle_ = JSON::Get<Float>(value);
							continue;
						}

						if (key == "outerConeAngle")
						{
							spot->outer_angle_ = JSON::Get<Float>(value);
							continue;
						}

						assert(false);
					}

					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					light.name_ = JSON::Get<std::string_view>(value);
					continue;
				}

				assert(false);
			}

			assert(type.has_value());

			if (*type == "directional")
			{
				assert(false == range.has_value());
				light.parameters_ = Light::Directional();
				continue;
			}

			if (*type == "point")
			{
				light.parameters_ = Light::Point { range };
				continue;
			}

			if (*type == "spot")
			{
				light.parameters_ = *spot;
				std::get<Light::Spot>(light.parameters_).range_ = range;
				continue;
			}

			assert(false);
		}

		return lights;
	}
}
