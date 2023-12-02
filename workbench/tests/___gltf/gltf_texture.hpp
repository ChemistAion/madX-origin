#pragma once

#include "gltf/_gltf.hpp"
#include "gltf/texture.hpp"

class gltfTexture : public Template
{
protected:
	simdjson::dom::parser parser_;
};

