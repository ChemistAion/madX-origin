#pragma once

#include "gltf/_gltf.hpp"
#include "gltf/material.hpp"

class gltfMaterial : public Template
{
protected:
	simdjson::dom::parser parser_;
};

