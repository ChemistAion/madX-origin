#include "_tests.hpp"

#include "gltf_image.hpp"

TEST_F(gltfImage, Cube)
{
	auto source = parser_.load("assets\\cube\\cube.gltf");
	ASSERT_FALSE(source.error());

	auto element = source.at_key("images");
	ASSERT_FALSE(element.error());

	////////////////////////////////////////////////////////////////////////////////

	auto images = glTF::GetImages(element.value());

	ASSERT_EQ(images.size(), 2);

	ASSERT_TRUE(std::holds_alternative<glTF::Image::DataURI>(images[0].data_));
	ASSERT_EQ(std::get<glTF::Image::DataURI>(images[0].data_).uri_, "Cube_BaseColor.png");

	ASSERT_TRUE(std::holds_alternative<glTF::Image::DataURI>(images[1].data_));
	ASSERT_EQ(std::get<glTF::Image::DataURI>(images[1].data_).uri_, "Cube_MetallicRoughness.png");

	return;
}