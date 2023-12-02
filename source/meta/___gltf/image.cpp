#include "_meta.hpp"

#include "image.hpp"

namespace madX::Meta::glTF
{
	Images GetImages(const simdjson::dom::element& element)
	{
		Images images;

		const auto& items = JSON::Get<simdjson::dom::array>(element);
		images.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = JSON::Get<simdjson::dom::object>(item);
			auto& image = images.emplace_back();

			std::optional<std::string> uri;
			std::optional<Index> buffer_view;
			std::optional<std::string> mime_type;

			for (const auto [key, value] : object)
			{
				if (key == "uri")
				{
					uri = JSON::Get<std::string_view>(value);
					continue;
				}

				if (key == "mimeType")
				{
					mime_type = JSON::Get<std::string_view>(value);
					continue;
				}

				if (key == "bufferView")
				{
					buffer_view = JSON::Get<Index>(value);
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					image.name_ = JSON::Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					continue;
				}

				if (key == "extras")
				{
					image.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}

			////////////////////////////////////////////////////////////////////////////////

			if (uri.has_value())
			{
				image.data_ = Image::DataURI{ *uri, {} };
				continue;
			}

			if (buffer_view.has_value())
			{
				assert(mime_type.has_value());
				image.data_ = Image::DataBufferView{ *mime_type, *buffer_view };
				continue;
			}

			assert(false);
		}

		return images;
	}
}
