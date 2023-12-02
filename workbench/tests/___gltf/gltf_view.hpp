#pragma once

#include "gltf/_gltf.hpp"
#include "gltf/view.hpp"

class gltfView : public Template
{
protected:
	simdjson::dom::parser parser_;
};
