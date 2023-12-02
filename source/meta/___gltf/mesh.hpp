#pragma once

namespace madX::Meta::glTF
{
	struct Mesh
	{
		struct Primitive
		{
			//	struct Attribute
			//	{
			//		std::string id_;
			//		Index accessor_;
			//	};

			enum class Mode
			{
				Points = 0,
				Lines = 1,
				LineLoop = 2,
				LineStrip = 3,
				Triangles = 4,
				TriangleStrip = 5,
				TriangleFan = 6
			};

			//	std::vector<Attribute> attributes_;
			std::map<std::string, Index> attributes_;
			std::optional<Index> indices_;
			std::optional<Index> material_;
			Mode mode_ = Mode::Triangles;

			Extras extras_;
		};

		////////////////////////////////////////////////////////////////////////////////

		std::vector<Primitive> primitives_;
		//std::vector<float> weights_;

		Name name_;
		Extras extras_;
	};

	using Meshes = std::vector<Mesh>;
	Meshes GetMeshes(const simdjson::dom::element& element);
}