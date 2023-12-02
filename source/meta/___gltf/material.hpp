#pragma once

namespace madX::Meta::glTF
{
	struct Material
	{
		struct Texture
		{
			Index index_;
			Index coord_ = 0;
			
			Extras extras_;
		};

		struct Normal
		{
			Index index_;
			Index coord_ = 0;
			Float scale_ = 1.0f;

			Extras extras_;
		};

		struct Occlusion
		{
			Index index_;
			Index coord_ = 0;
			Float strength_ = 1.0f;

			Extras extras_;
		};

		struct PBR
		{
			XMFLOAT4 base_factor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
			std::optional<Texture> base_texture_;
			Float metallic_factor_ = 1.0f;
			Float roughness_factor = 1.0f;
			std::optional<Texture> mr_texture_;

			Extras extras_;
		};

		enum class AlphaMode
		{
			None,
			Opaque,
			Mask,
			Blend
		};

		////////////////////////////////////////////////////////////////////////////////

		std::optional<PBR> pbr_;
		std::optional<Normal> normal_;
		std::optional<Occlusion> occlusion_;
		std::optional<Texture> emissive_;
		XMFLOAT3 emissive_factor_;
		AlphaMode alpha_mode_ = AlphaMode::None;
		Float alpha_cutoff_ = 0.5f;
		Bool double_sided = false;

		Name name_;
		Extras extras_;

		static Texture GetTexture(const simdjson::dom::object&);
		static Texture GetNormal(const simdjson::dom::object&);
	};

	using Materials = std::vector<Material>;
	Materials GetMaterials(const simdjson::dom::element& element);


}