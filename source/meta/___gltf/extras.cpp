#include "_meta.hpp"

namespace madX::Meta::glTF
{
	Extras GetExtras(const simdjson::dom::element& element)
	{
		return JSON::GetValue(element);
	}
}
