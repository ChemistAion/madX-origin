#pragma once

//https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md
//https://simdjson.org/api/0.8.0/index.html

#include "scene.hpp"
#include "node.hpp"

#include "skin.hpp"
#include "camera.hpp"
#include "mesh.hpp"

#include "accessor.hpp"
#include "view.hpp"
#include "buffer.hpp"

#include "material.hpp"
#include "texture.hpp"
#include "image.hpp"
#include "sampler.hpp"

#include "animation.hpp"

#include "asset.hpp"

//#include "light.hpp"

namespace madX::Meta::glTF
{
	struct Model
	{
		Scenes scenes_;
		Nodes nodes_;

		Skins skins_;
		Cameras cameras_;
		Meshes meshes_;

		Accessors accessors_;
		Views views_;
		Buffers buffers_;

		Materials materials_;
		Textures textures_;
		Images images_;
		Samplers samplers_;

		//Lights lights_;
		Animations animations_;

		Asset asset_;

		bool Load(const std::string& file);
	};
}