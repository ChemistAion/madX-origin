#include "_tests.hpp"

#include "gltf_scene.hpp"

TEST_F(gltfScene, Cube)
{
	auto source = parser_.load("assets\\cube\\cube.gltf");
	ASSERT_FALSE(source.error());

	auto element = source.at_key("scenes");
	ASSERT_FALSE(element.error());

	////////////////////////////////////////////////////////////////////////////////

	auto scenes = glTF::GetScenes(element.value());

	ASSERT_EQ(scenes.size(), 1);

	ASSERT_EQ(scenes[0].nodes_.size(), 1);
	ASSERT_EQ(scenes[0].nodes_[0], 0);

	return;
}