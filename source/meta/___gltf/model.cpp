#include "_meta.hpp"

#include "model.hpp"

namespace madX::Meta::glTF
{
    bool Model::Load(const std::string& file)
    {
		std::filesystem::path path(file);
		assert(std::filesystem::exists(path));

		simdjson::dom::parser parser_;
		simdjson::dom::element source;

		if (auto error = parser_.load(file).get(source); error)
		{
			return false;
		}

		////////////////////////////////////////////////////////////////////////////////

		if (auto result = source.at_key("scenes"); false == result.error())
		{
			scenes_ = std::move(GetScenes(result.value()));
		}

		if (auto result = source.at_key("nodes"); false == result.error())
		{
			nodes_ = std::move(GetNodes(result.value()));
		}

		////////////////////////////////////////////////////////////////////////////////

		if (auto result = source.at_key("skins"); false == result.error())
		{
			skins_ = std::move(GetSkins(result.value()));
		}
		
		if (auto result = source.at_key("camers"); false == result.error())
		{
			cameras_ = std::move(GetCameras(result.value()));
		}

		if (auto result = source.at_key("meshes"); false == result.error())
		{
			meshes_ = std::move(GetMeshes(result.value()));
		}

		////////////////////////////////////////////////////////////////////////////////

		if ( auto result = source.at_key("accessors"); false == result.error())
		{
			accessors_ = std::move(GetAccessors(result.value()));
		}

		if (auto result = source.at_key("bufferViews"); false == result.error())
		{
			views_ = std::move(GetViews(result.value()));
		}

		if (auto result = source.at_key("buffers"); false == result.error())
		{
			buffers_ = std::move(GetBuffers(result.value()));
		}

		////////////////////////////////////////////////////////////////////////////////

		if (auto result = source.at_key("materials"); false == result.error())
		{
			materials_ = std::move(GetMaterials(result.value()));
		}
		
		if (auto result = source.at_key("textures"); false == result.error())
		{
			textures_ = std::move(GetTextures(result.value()));
		}
		
		if (auto result = source.at_key("images"); false == result.error())
		{
			images_ = std::move(GetImages(result.value()));
		}

		if (auto result = source.at_key("samplers"); false == result.error())
		{
			samplers_ = std::move(GetSamplers(result.value()));
		}

		////////////////////////////////////////////////////////////////////////////////

		if (auto result = source.at_key("animations"); false == result.error())
		{
			animations_ = std::move(GetAnimations(result.value()));
		}

		////////////////////////////////////////////////////////////////////////////////

		if (auto result = source.at_key("asset"); false == result.error())
		{
			asset_ = std::move(GetAsset(result.value()));
		}

		//lights

		////////////////////////////////////////////////////////////////////////////////

		for (auto& buffer : buffers_)
		{
			if (buffer.uri_.has_value())
			{
				buffer.data_ = GetData(path.parent_path(), buffer.uri_.value());
			}
		}

		for (auto& image : images_)
		{
			if (auto uri = std::get_if<Image::DataURI>(&image.data_))
			{
				uri->data_ = GetData(path.parent_path(), uri->uri_);
			}
		}

		return true;
    }
}
