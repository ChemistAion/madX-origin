#include "_meta.hpp"

#include "mesh.hpp"

namespace madX::Meta::glTF
{
	Meshes GetMeshes(const simdjson::dom::element& element)
	{
		Meshes meshes;

		const auto& items = JSON::Get<simdjson::dom::array>(element);
		meshes.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = JSON::Get<simdjson::dom::object>(item);
			auto& mesh = meshes.emplace_back();

			for (const auto [key, value] : object)
			{
				if (key == "primitives")
				{
					const auto& items = JSON::Get<simdjson::dom::array>(value);
					mesh.primitives_.reserve(items.size());

					//TODO: rename these
					for (const auto item : items)
					{
						const auto object = JSON::Get<simdjson::dom::object>(item);
						auto& primitive = mesh.primitives_.emplace_back();

						for (const auto [key, value] : object)
						{
							if (key == "attributes")
							{
								const auto object = JSON::Get<simdjson::dom::object>(value);
								for (const auto [key, value] : object)
								{

								//	primitive.attributes_.push_back
								//	(
								//		Mesh::Primitive::Attribute
								//		{
								//			std::string(key),
								//			JSON::Get<Index>(value)
								//		}
								//	);
									primitive.attributes_.emplace(std::string(key), JSON::Get<Index>(value));
								}

								continue;
							}

							if (key == "indices")
							{
								primitive.indices_ = JSON::Get<Index>(value);
								continue;
							}

							if (key == "material")
							{
								primitive.material_ = JSON::Get<Index>(value);
								continue;
							}

							if (key == "mode")
							{
								primitive.mode_ = static_cast<Mesh::Primitive::Mode>(JSON::Get<UnsignedInteger>(value));
								continue;
							}

							if (key == "targets")
							{
								assert(false);
								continue;
							}

							if (key == "extensions")
							{
								assert(false);
								continue;
							}

							if (key == "extras")
							{
								primitive.extras_ = std::move(GetExtras(value));
								continue;
							}

							assert(false);
						}
					}

					continue;
				}

				if (key == "weights")
				{
					assert(false);
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					mesh.name_ = JSON::Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					assert(false);
					continue;
				}

				if (key == "extras")
				{
					mesh.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		}

		return meshes;
	}
}
