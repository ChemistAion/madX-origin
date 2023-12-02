#include "_tests.hpp"

#include "gltf_view.hpp"

TEST_F(gltfView, Cube)
{
	auto source = parser_.load("assets\\cube\\cube.gltf");
	ASSERT_FALSE(source.error());

	auto element = source.at_key("bufferViews");
	ASSERT_FALSE(element.error());

	////////////////////////////////////////////////////////////////////////////////

	auto views = glTF::GetViews(element.value());

	ASSERT_EQ(views.size(), 5);

	ASSERT_EQ(views[0].size_, 72);
	ASSERT_EQ(views[0].offset_, 0);
	ASSERT_EQ(views[0].target_, glTF::View::Target::ElementArrayBuffer);

	ASSERT_EQ(views[1].size_, 432);
	ASSERT_EQ(views[1].offset_, 72);
	ASSERT_EQ(views[1].target_, glTF::View::Target::ArrayBuffer);

	ASSERT_EQ(views[2].size_, 432);
	ASSERT_EQ(views[2].offset_, 504);
	ASSERT_EQ(views[2].target_, glTF::View::Target::ArrayBuffer);

	ASSERT_EQ(views[3].size_, 576);
	ASSERT_EQ(views[3].offset_, 936);
	ASSERT_EQ(views[3].target_, glTF::View::Target::ArrayBuffer);

	ASSERT_EQ(views[4].size_, 288);
	ASSERT_EQ(views[4].offset_, 1512);
	ASSERT_EQ(views[4].target_, glTF::View::Target::ArrayBuffer);

	return;
}