#pragma once

#include "meta/gltf/data.hpp"
#include "meta/gltf/accessor.hpp"
#include "meta/gltf/buffer.hpp"
#include "meta/gltf/view.hpp"
#include "meta/gltf/mesh.hpp"
#include "meta/gltf/node.hpp"
#include "meta/gltf/image.hpp"
#include "meta/gltf/texture.hpp"
#include "meta/gltf/sampler.hpp"
#include "meta/gltf/material.hpp"

namespace madX::Meta::glTF
{
	class Source
	{
		simdjson::dom::parser parser_;
		simdjson::simdjson_result<simdjson::dom::element> source_;

		Accessors accessors_;
		Buffers buffers_;
		Views views_;

		Nodes nodes_;
		Meshes meshes_;

		Images images_;
		Textures textures_;
		Samplers samplers_;
		Materials materials_;
	public:
		Source(const std::filesystem::path& path);

		////////////////////////////////////////////////////////////////////////////////

		const auto& GetAccessors() const noexcept
		{
			return accessors_;
		}

		const auto& GetBuffers() const noexcept
		{
			return buffers_;
		}

		const auto& GetViews() const noexcept
		{
			return views_;
		}

		const auto& GetMeshes() const noexcept
		{
			return meshes_;
		}

		const auto& GetNodes() const noexcept
		{
			return nodes_;
		}

		const auto& GetImages() const noexcept
		{
			return images_;
		}

		const auto& GetTextures() const noexcept
		{
			return textures_;
		}

		const auto& GetSamplers() const noexcept
		{
			return samplers_;
		}

		const auto& GetMaterials() const noexcept
		{
			return materials_;
		}
	};
}