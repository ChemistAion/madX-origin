#pragma once

#include "gltf/_gltf.hpp"
#include "gltf/asset.hpp"

class gltfAsset : public Template
{
protected:
	simdjson::dom::parser parser_;
};

