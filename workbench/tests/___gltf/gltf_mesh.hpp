#pragma once

#include "gltf/_gltf.hpp"
#include "gltf/mesh.hpp"

class gltfMesh : public Template
{
protected:
	simdjson::dom::parser parser_;
};

