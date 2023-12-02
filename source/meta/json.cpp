#include "json.hpp"

namespace madX::Meta::JSON
{
	Value GetValue(const simdjson::dom::element& element) noexcept
	{
		switch (element.type())
		{
		case simdjson::dom::element_type::NULL_VALUE:
			return Null{};
		case simdjson::dom::element_type::STRING:
			//return String(std::string_view(element));
			return String(element);
		case simdjson::dom::element_type::INT64:
			return std::int64_t(element);
		case simdjson::dom::element_type::UINT64:
			return std::uint64_t(element);
		case simdjson::dom::element_type::DOUBLE:
			return double(element);
		case simdjson::dom::element_type::BOOL:
			return bool(element);
		case simdjson::dom::element_type::ARRAY:
		{
			Value value = std::make_unique<Array>();
			auto& array = *std::get<std::unique_ptr<Array>>(value);
	
			for (const auto& item : simdjson::dom::array(element))
			{
				array.emplace_back(GetValue(item));
			}
	
			return value;
		}
		case simdjson::dom::element_type::OBJECT:
		{
			Value value = std::make_unique<Object>();
			auto& object = *std::get<std::unique_ptr<Object>>(value);
	
			for (const auto& item : simdjson::dom::object(element))
			{
				object.emplace(item.key, GetValue(item.value));
			}
	
			return value;
		}
		default:
		{
			break;
		}
		}
	
		assert(false);
		return Null{};
	}

	////////////////////////////////////////////////////////////////////////////////

	Meta::Value GetMetaValue(const simdjson::dom::element& element) noexcept
	{
		switch (element.type())
		{
		case simdjson::dom::element_type::NULL_VALUE:
			return Meta::Value { Meta::Null() };
		case simdjson::dom::element_type::STRING:
			return Meta::Value { Meta::String(element) };
		case simdjson::dom::element_type::INT64:
			return Meta::Value { std::int64_t(element) };
		case simdjson::dom::element_type::UINT64:
			return Meta::Value { std::uint64_t(element) };
		case simdjson::dom::element_type::DOUBLE:
			return Meta::Value { double(element) };
		case simdjson::dom::element_type::BOOL:
			return Meta::Value { bool(element) };
		case simdjson::dom::element_type::ARRAY:
		{
			MetaArray meta = Meta::MakeUniquePtr<Meta::Array>();
			
			for (const auto& item : simdjson::dom::array(element))
			{
				meta->emplace_back(GetMetaValue(item));
			}
			
			return Meta::Value { std::move(meta) };
		}
		case simdjson::dom::element_type::OBJECT:
		{
			MetaObject meta = Meta::MakeUniquePtr<Meta::Object>();
			
			for (const auto& item : simdjson::dom::object(element))
			{
				meta->emplace(Meta::String(item.key), GetMetaValue(item.value));
			}
			
			return Meta::Value { std::move(meta) };
		}
		default:
		{
			break;
		}
		}

		assert(false);
		return Meta::Value { Meta::Null() };
	}

	Meta::Object GetMetaObject(const Core::Filesystem::Stream& stream) noexcept
	{
		simdjson::dom::parser parser;
		simdjson::padded_string json(stream.Get());

		auto document = parser.parse(json);

		if (document.error())
		{
			//TODO: error handling
			//assert(false);
			return {};
		}

		auto root = document.get_object();

		if (root.error())
		{
			//TODO: error handling
			//assert(false);
			return {};
		}

		////////////////////////////////////////////////////////////////////////////////

		Meta::Object object;

		for (auto [key, value] : root)
		{
			auto element = object.emplace(Meta::String(key), GetMetaValue(value));

			if (false == element.second)
			{
				//TODO: error handling
				//assert(false);
				continue;
			}
		}

		return object;
	}
}
