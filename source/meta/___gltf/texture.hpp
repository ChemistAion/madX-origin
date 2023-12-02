#pragma once

namespace madX::Meta::glTF
{
	struct Texture
	{
		std::optional<Index> sampler_;
		std::optional<Index> source_;

		Name name_;
		Extras extras_;
	};

	using Textures = std::vector<Texture>;

	Textures GetTextures(const simdjson::dom::element& element);
}