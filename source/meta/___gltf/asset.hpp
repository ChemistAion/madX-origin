#pragma once

namespace madX::Meta::glTF
{
	struct Asset
	{
		std::string copyright_;
		std::string generator_;
		std::string version_;
		std::string compatibility_;

		Extras extras_;
	};

	Asset GetAsset(const simdjson::dom::element& element);
}