import <cassert>;

import stl.core;
import madX.Core.Filesystem;
import utils.simdjson;
import madX.Meta.Value;

namespace madX::Meta::JSON
{
	////////////////////////////////////////////////////////////////////////////////

	struct Array;
	struct Object;
	
	using Null = std::monostate;
	using String = std::string;

	using Value = std::variant
	<
		Null,
		String,
		std::int64_t,
		std::uint64_t,
		double,
		bool,
		std::unique_ptr<Array>,
		std::unique_ptr<Object>
	>;
	
	struct Array : public std::vector<Value>
	{
	};
	
	struct Object : public std::unordered_map<String, Value>
	{
	};

	////////////////////////////////////////////////////////////////////////////////

	// http://json-schema.org/draft/2020-12/json-schema-validation.html#rfc.section.6.1.1
	// https://stackoverflow.com/questions/43762617/is-there-an-unsigned-integer-in-json-schema

	//TODO: remove float handling
	template <typename Type>
	Type Get(const simdjson::dom::element& element)
	{
		if constexpr (std::is_arithmetic_v<Type>)
		{
			if (element.is_number())
			{
				if constexpr (std::is_floating_point_v<Type>)
				{				
					if (auto result = element.get_double(); false == result.error())
					{
						return Type(result.value());
					}
				}
				else if constexpr (std::is_integral_v<Type> && std::is_signed_v<Type>)
				{
					if (auto result = element.get_int64(); false == result.error())
					{
						return Type(result.value());
					}
				}
				else if constexpr (std::is_integral_v<Type> && std::is_unsigned_v<Type>)
				{
					if (auto result = element.get_uint64(); false == result.error())
					{
						return Type(result.value());
					}
				}
			}		
			else if (element.is_bool())
			{
				if constexpr (std::is_same_v<Type, bool>)
				{
					if (auto result = element.get_bool(); false == result.error())
					{
						return Type(result.value());
					}
				}
			}
		}
		else
		{
			if (auto result = element.get<Type>(); false == result.error())
			{
				return result.value();
			}
		}

		assert(false);
		return {};
	}
	
	//TODO: optimize this, make it at once
	template <typename Type>
	std::vector<Type> GetVector(const simdjson::dom::element& element)
	{
		std::vector<Type> vector;
	
		const auto items = Get<simdjson::dom::array>(element);
		vector.reserve(items.size());
	
		for (const auto item : items)
		{
			vector.emplace_back(Get<Type>(item));
		}
	
		return vector;
	}

	////////////////////////////////////////////////////////////////////////////////

	Value GetValue(const simdjson::dom::element& element) noexcept;

	Meta::Value GetMetaValue(const simdjson::dom::element& element) noexcept;
	Meta::Object GetMetaObject(const Core::Filesystem::Stream& stream) noexcept;
}