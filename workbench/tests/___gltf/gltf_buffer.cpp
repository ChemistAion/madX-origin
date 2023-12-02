#include "_tests.hpp"

#include "gltf_buffer.hpp"

TEST_F(gltfBuffer, Cube)
{
	auto source = parser_.load("assets\\cube\\cube.gltf");
	ASSERT_FALSE(source.error());

	auto element = source.at_key("buffers");
	ASSERT_FALSE(element.error());

	////////////////////////////////////////////////////////////////////////////////

	auto buffers = glTF::GetBuffers(element.value());

	ASSERT_EQ(buffers.size(), 1);

	ASSERT_EQ(buffers[0].size_, 1800);
	ASSERT_TRUE(buffers[0].uri_.value() == "Cube.bin");
	
	return;
}