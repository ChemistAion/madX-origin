#include "_tests.hpp"

#include "gltf_texture.hpp"

TEST_F(gltfTexture, Cube)
{
	auto source = parser_.load("assets\\cube\\cube.gltf");
	ASSERT_FALSE(source.error());

	auto element = source.at_key("textures");
	ASSERT_FALSE(element.error());

	////////////////////////////////////////////////////////////////////////////////

	auto textures = glTF::GetTextures(element.value());

	ASSERT_EQ(textures.size(), 2);

	ASSERT_EQ(textures[0].sampler_, 0);
	ASSERT_EQ(textures[0].source_, 0);

	ASSERT_EQ(textures[1].sampler_, 0);
	ASSERT_EQ(textures[1].source_, 1);

	return;
}