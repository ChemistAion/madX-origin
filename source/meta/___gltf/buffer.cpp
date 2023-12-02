#include "_meta.hpp"

#include "buffer.hpp"

namespace madX::Meta::glTF
{
	Buffers GetBuffers(const simdjson::dom::element& element)
	{
		Buffers buffers;

		const auto& items = JSON::Get<simdjson::dom::array>(element);
		buffers.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = JSON::Get<simdjson::dom::object>(item);
			auto& buffer = buffers.emplace_back();

			for (const auto [key, value] : object)
			{
				if (key == "uri")
				{
					buffer.uri_ = JSON::Get<std::string_view>(value);
					continue;
				}

				if (key == "byteLength")
				{
					buffer.size_ = JSON::Get<Size>(value);
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					buffer.name_ = JSON::Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					continue;
				}

				if (key == "extras")
				{
					buffer.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		}

		return buffers;
	}
}
