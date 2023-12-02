#pragma once

namespace madX::Meta::glTF
{
	struct Skin
	{
		std::optional<Index> ibm_;
		std::optional<Index> skeleton_;
		std::vector<Index> joints_;

		Name name_;
		Extras extras_;
	};

	using Skins = std::vector<Skin>;
	Skins GetSkins(const simdjson::dom::element& element);
}