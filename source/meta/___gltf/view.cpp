#include "_meta.hpp"

#include "view.hpp"

namespace madX::Meta::glTF
{
	Views GetViews(const simdjson::dom::element& element)
	{
		Views views;

		const auto& items = JSON::Get<simdjson::dom::array>(element);
		views.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = JSON::Get<simdjson::dom::object>(item);
			auto& view = views.emplace_back();

			for (const auto [key, value] : object)
			{
				if (key == "buffer")
				{
					view.buffer_= JSON::Get<Index>(value);
					continue;
				}

				if (key == "byteOffset")
				{
					view.offset_ = JSON::Get<Offset>(value);
					continue;
				}

				if (key == "byteLength")
				{
					view.size_ = JSON::Get<Size>(value);
					continue;
				}

				if (key == "byteStride")
				{
					view.stride_ = JSON::Get<Size>(value);
					continue;
				}

				if (key == "target")
				{
					view.target_ = static_cast<View::Target>(JSON::Get<Type>(value));
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					view.name_ = JSON::Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					continue;
				}

				if (key == "extras")
				{
					view.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		}

		return views;
	}
}
