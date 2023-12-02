#include "_tests.hpp"

#include "gltf_model.hpp"

TEST_F(gltfModel, Cube)
{
	glTF::Model model;

	auto result = model.Load("assets\\cube\\cube.gltf");

	return;
}