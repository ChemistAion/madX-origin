#include "_meta.hpp"

#include "sampler.hpp"

namespace madX::Meta::glTF
{
	Samplers GetSamplers(const simdjson::dom::element& element)
	{
		Samplers samplers;

		const auto& items = JSON::Get<simdjson::dom::array>(element);
		samplers.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = JSON::Get<simdjson::dom::object>(item);
			auto& sampler = samplers.emplace_back();

			for (const auto [key, value] : object)
			{
				if (key == "minFilter")
				{
					sampler.min_filter_ = static_cast<Sampler::MinFilter>(JSON::Get<UnsignedInteger>(value));
					continue;
				}

				if (key == "magFilter")
				{
					sampler.mag_filter_ = static_cast<Sampler::MagFilter>(JSON::Get<UnsignedInteger>(value));
					continue;
				}

				if (key == "wrapS")
				{
					sampler.wrap_s_ = static_cast<Sampler::WrapMode>(JSON::Get<UnsignedInteger>(value));
					continue;
				}

				if (key == "wrapT")
				{
					sampler.wrap_t_ = static_cast<Sampler::WrapMode>(JSON::Get<UnsignedInteger>(value));
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					sampler.name_ = JSON::Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					continue;
				}

				if (key == "extras")
				{
					sampler.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		}

		return samplers;
	}
}

