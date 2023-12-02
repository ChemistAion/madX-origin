#pragma once

#include "gltf/_gltf.hpp"
#include "gltf/scene.hpp"

class gltfScene : public Template
{
protected:
	simdjson::dom::parser parser_;
};
