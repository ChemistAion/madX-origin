#pragma once

namespace madX::Meta::glTF
{
	using Extras = JSON::Value;

	Extras GetExtras(const simdjson::dom::element& element);
}