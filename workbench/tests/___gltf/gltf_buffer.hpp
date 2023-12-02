#pragma once

#include "gltf/_gltf.hpp"
#include "gltf/buffer.hpp"

class gltfBuffer : public Template
{
protected:
	simdjson::dom::parser parser_;
};
