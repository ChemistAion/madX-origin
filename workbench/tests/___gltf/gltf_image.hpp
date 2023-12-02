#pragma once

#include "gltf/_gltf.hpp"
#include "gltf/image.hpp"

class gltfImage : public Template
{
protected:
	simdjson::dom::parser parser_;
};

