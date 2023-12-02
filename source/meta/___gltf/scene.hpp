#pragma once

namespace madX::Meta::glTF
{
	struct Scene
	{
		std::vector<Index> nodes_;

		Name name_;
		Extras extras_;
	};

	using Scenes = std::vector<Scene>;

	Scenes GetScenes(const simdjson::dom::element& element);
}