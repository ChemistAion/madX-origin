#include "_tests.hpp"

#include "gltf_mesh.hpp"

TEST_F(gltfMesh, Cube)
{
	auto source = parser_.load("assets\\cube\\cube.gltf");
	ASSERT_FALSE(source.error());

	auto element = source.at_key("meshes");
	ASSERT_FALSE(element.error());

	////////////////////////////////////////////////////////////////////////////////

	auto meshes = glTF::GetMeshes(element.value());

	ASSERT_EQ(meshes.size(), 1);

	ASSERT_EQ(meshes[0].name_, "Cube");

	ASSERT_EQ(meshes[0].primitives_.size(), 1);
	ASSERT_EQ(meshes[0].primitives_[0].attributes_.size(), 4);
	ASSERT_EQ(meshes[0].primitives_[0].attributes_[0].id_, "NORMAL");
	ASSERT_EQ(meshes[0].primitives_[0].attributes_[0].accessor_, 2);
	ASSERT_EQ(meshes[0].primitives_[0].attributes_[1].id_, "POSITION");
	ASSERT_EQ(meshes[0].primitives_[0].attributes_[1].accessor_, 1);
	ASSERT_EQ(meshes[0].primitives_[0].attributes_[2].id_, "TANGENT");
	ASSERT_EQ(meshes[0].primitives_[0].attributes_[2].accessor_, 3);
	ASSERT_EQ(meshes[0].primitives_[0].attributes_[3].id_, "TEXCOORD_0");
	ASSERT_EQ(meshes[0].primitives_[0].attributes_[3].accessor_, 4);
	ASSERT_EQ(meshes[0].primitives_[0].indices_, 0);
	ASSERT_EQ(meshes[0].primitives_[0].material_, 0);
	ASSERT_EQ(meshes[0].primitives_[0].mode_, glTF::Mesh::Primitive::Mode::Triangles);

	return;
}