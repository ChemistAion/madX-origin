#include "_tests.hpp"

#include "gltf_asset.hpp"

TEST_F(gltfAsset, Cube)
{
	auto source = parser_.load("assets\\cube\\cube.gltf");
	ASSERT_FALSE(source.error());

	auto element = source.at_key("asset");
	ASSERT_FALSE(element.error());

	////////////////////////////////////////////////////////////////////////////////

//	auto asset = glTF::GetAsset(element.value());
//	
//	ASSERT_EQ(asset.generator_, "VKTS glTF 2.0 exporter");
//	ASSERT_EQ(asset.version_, "2.0");
//	
//	ASSERT_EQ(asset.extras_.index(), 0);

	return;
}