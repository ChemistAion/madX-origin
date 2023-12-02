#include "_meta.hpp"

#include "material.hpp"

namespace madX::Meta::glTF
{
	Material::Texture Material::GetTexture(const simdjson::dom::object& object)
	{
		Material::Texture texture;

		for (const auto [key, value] : object)
		{
			if (key == "index")
			{
				texture.index_ = JSON::Get<Index>(value);
				continue;
			}

			if (key == "texCoord")
			{
				texture.coord_ = JSON::Get<Coord>(value);
				continue;
			}

			continue;
		}

		return texture;
	}

	Material::Normal GetNormal(const simdjson::dom::object& object)
	{
		Material::Normal normal;

		for (const auto [key, value] : object)
		{
			if (key == "index")
			{
				normal.index_ = JSON::Get<Index>(value);
				continue;
			}

			if (key == "texCoord")
			{
				normal.coord_ = JSON::Get<Coord>(value);
				continue;
			}

			continue;
		}

		return normal;
	}

	Materials GetMaterials(const simdjson::dom::element& element)
	{
		Materials materials;

		const auto& items = JSON::Get<simdjson::dom::array>(element);
		materials.reserve(items.size());

		for (const auto item : items)
		{
			//const auto object = JSON::Get<simdjson::dom::object>(item);
			auto& material = materials.emplace_back();

			for (const auto [key, value] : JSON::Get<simdjson::dom::object>(item))
			{
				if (key == "normalTexture")
				{
					const auto object = JSON::Get<simdjson::dom::object>(value);
					material.normal_ = GetNormal(object);
					continue;
				}

				if (key == "occlusionTexture")
				{
					material.occlusion_ = Material::Occlusion();
					continue;
				}

				if (key == "emissiveTexture")
				{
					material.emissive_ = Material::Texture();
					continue;
				}

				//TODO: TextureInfo
				if (key == "pbrMetallicRoughness")
				{
					auto& pbr = material.pbr_ = Material::PBR();

					//const auto object = JSON::Get<simdjson::dom::object>(value);

					for (const auto [key, value] : JSON::Get<simdjson::dom::object>(value))
					{
						if (key == "baseColorTexture")
						{
							const auto object = JSON::Get<simdjson::dom::object>(value);
							pbr->base_texture_ = Material::GetTexture(object);
							continue;
						}

						if (key == "metallicRoughnessTexture")
						{
							const auto object = JSON::Get<simdjson::dom::object>(value);
							pbr->mr_texture_ = Material::GetTexture(object);
							continue;
						}
					}
					continue;
				}

				if (key == "emissiveFactor")
				{
					auto vector = JSON::GetVector<Float>(value);
					assert(vector.size() == 3);

					material.emissive_factor_ = XMFLOAT3(vector.data());
					continue;
				}

				if (key == "alphaMode")
				{
					const auto mode = JSON::Get<std::string_view>(value);

					if (mode == "OPAQUE")
					{
						material.alpha_mode_ = Material::AlphaMode::Opaque;
						continue;
					}

					if (mode == "MASK")
					{
						material.alpha_mode_ = Material::AlphaMode::Mask;
						continue;
					}

					if (mode == "BLEND")
					{
						material.alpha_mode_ = Material::AlphaMode::Blend;
						continue;
					}

					assert(false);
				}

				if (key == "alphaCutoff")
				{
					material.alpha_cutoff_ = JSON::Get<Float>(value);
					continue;
				}

				if (key == "doubleSided")
				{
					material.double_sided = JSON::Get<Bool>(value);
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					material.name_ = JSON::Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					continue;
				}

				if (key == "extras")
				{
					material.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		}

		return materials;
	}
}
