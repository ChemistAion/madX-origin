#include "_tests.hpp"

#include "mockup_model.hpp"

TEST_F(MockupModel, Cube)
{
	glTF::Model gltf;
	
	auto result = gltf.Load("assets\\cube\\cube.gltf");
	ASSERT_TRUE(result);

	Mockup::Model model;
	model.Import(gltf);

	return;
}