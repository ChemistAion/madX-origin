#pragma once

#include "gltf/_gltf.hpp"
#include "gltf/accessor.hpp"

class gltfAccessor : public Template
{
protected:
	simdjson::dom::parser parser_;
};
