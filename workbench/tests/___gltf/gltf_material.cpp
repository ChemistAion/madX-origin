#include "_tests.hpp"

#include "gltf_material.hpp"

TEST_F(gltfMaterial, Cube)
{
	auto source = parser_.load("assets\\cube\\cube.gltf");
	ASSERT_FALSE(source.error());

	auto element = source.at_key("materials");
	ASSERT_FALSE(element.error());

	////////////////////////////////////////////////////////////////////////////////

	auto materials = glTF::GetMaterials(element.value());

	ASSERT_EQ(materials.size(), 1);

	ASSERT_EQ(materials[0].name_, "Cube");
	//ASSERT_EQ(materials[0].pbr_, "Cube"); //TODO...

	return;
}