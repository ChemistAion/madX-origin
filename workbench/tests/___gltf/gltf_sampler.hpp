#pragma once

#include "gltf/_gltf.hpp"
#include "gltf/sampler.hpp"

class gltfSampler: public Template
{
protected:
	simdjson::dom::parser parser_;
};

