#include "_tests.hpp"

#include "gltf_accessor.hpp"

TEST_F(gltfAccessor, Cube)
{
	auto source = parser_.load("assets\\cube\\cube.gltf");
	ASSERT_FALSE(source.error());

	auto element = source.at_key("accessors");
	ASSERT_FALSE(element.error());

	////////////////////////////////////////////////////////////////////////////////

	auto accessors = glTF::GetAccessors(element.value());

	ASSERT_EQ(accessors.size(), 5);

	ASSERT_EQ(accessors[0].buffer_view_, 0);
	ASSERT_EQ(accessors[0].offset_, 0);
	//ASSERT_EQ(accessors[0].component_type_, glTF::Accessor::ComponentType::UnsignedShort);
	ASSERT_EQ(accessors[0].count_, 36);
	ASSERT_EQ(accessors[0].max_.size(), 1);
	ASSERT_EQ(accessors[0].max_[0], 35);
	ASSERT_EQ(accessors[0].min_.size(), 1);
	ASSERT_EQ(accessors[0].min_[0], 0);
	//ASSERT_EQ(accessors[0].type_, glTF::Accessor::Type::Scalar);

	ASSERT_EQ(accessors[1].buffer_view_, 1);
	ASSERT_EQ(accessors[1].offset_, 0);
	//ASSERT_EQ(accessors[1].component_type_, glTF::Accessor::ComponentType::Float);
	ASSERT_EQ(accessors[1].count_, 36);
	ASSERT_EQ(accessors[1].max_.size(), 3);
	ASSERT_EQ(accessors[1].max_[0], 1.000000);
	ASSERT_EQ(accessors[1].max_[1], 1.000000);
	ASSERT_EQ(accessors[1].max_[2], 1.000001);
	ASSERT_EQ(accessors[1].min_.size(), 3);
	ASSERT_EQ(accessors[1].min_[0], -1.000000);
	ASSERT_EQ(accessors[1].min_[1], -1.000000);
	ASSERT_EQ(accessors[1].min_[2], -1.000000);
	//ASSERT_EQ(accessors[1].type_, glTF::Accessor::Type::Vec3);

	ASSERT_EQ(accessors[2].buffer_view_, 2);
	ASSERT_EQ(accessors[2].offset_, 0);
	//ASSERT_EQ(accessors[2].component_type_, glTF::Accessor::ComponentType::Float);
	ASSERT_EQ(accessors[2].count_, 36);
	ASSERT_EQ(accessors[2].max_.size(), 3);
	ASSERT_EQ(accessors[2].max_[0], 1.000000);
	ASSERT_EQ(accessors[2].max_[1], 1.000000);
	ASSERT_EQ(accessors[2].max_[2], 1.000000);
	ASSERT_EQ(accessors[2].min_.size(), 3);
	ASSERT_EQ(accessors[2].min_[0], -1.000000);
	ASSERT_EQ(accessors[2].min_[1], -1.000000);
	ASSERT_EQ(accessors[2].min_[2], -1.000000);
	//ASSERT_EQ(accessors[2].type_, glTF::Accessor::Type::Vec3);

	ASSERT_EQ(accessors[3].buffer_view_, 3);
	ASSERT_EQ(accessors[3].offset_, 0);
	//ASSERT_EQ(accessors[3].component_type_, glTF::Accessor::ComponentType::Float);
	ASSERT_EQ(accessors[3].count_, 36);
	ASSERT_EQ(accessors[3].max_.size(), 4);
	ASSERT_EQ(accessors[3].max_[0], 1.000000);
	ASSERT_EQ(accessors[3].max_[1], -0.000000);
	ASSERT_EQ(accessors[3].max_[2], -0.000000);
	ASSERT_EQ(accessors[3].max_[3], 1.000000);
	ASSERT_EQ(accessors[3].min_.size(), 4);
	ASSERT_EQ(accessors[3].min_[0], 0.000000);
	ASSERT_EQ(accessors[3].min_[1], -0.000000);
	ASSERT_EQ(accessors[3].min_[2], -1.000000);
	ASSERT_EQ(accessors[3].min_[3], -1.000000);
	//ASSERT_EQ(accessors[3].type_, glTF::Accessor::Type::Vec4);

	ASSERT_EQ(accessors[4].buffer_view_, 4);
	ASSERT_EQ(accessors[4].offset_, 0);
	//ASSERT_EQ(accessors[4].component_type_, glTF::Accessor::ComponentType::Float);
	ASSERT_EQ(accessors[4].count_, 36);
	ASSERT_EQ(accessors[4].max_.size(), 2);
	ASSERT_EQ(accessors[4].max_[0], 1.000000);
	ASSERT_EQ(accessors[4].max_[1], 1.000000);
	ASSERT_EQ(accessors[4].min_.size(), 2);
	ASSERT_EQ(accessors[4].min_[0], -1.000000);
	ASSERT_EQ(accessors[4].min_[1], -1.000000);
	//ASSERT_EQ(accessors[4].type_, glTF::Accessor::Type::Vec2);

	return;
}