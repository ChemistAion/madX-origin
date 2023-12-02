#pragma once

#include "gltf/_gltf.hpp"
#include "gltf/node.hpp"

class gltfNode : public Template
{
protected:
	simdjson::dom::parser parser_;
};
