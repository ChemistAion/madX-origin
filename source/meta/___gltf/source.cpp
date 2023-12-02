#include "_meta.hpp"

#include "source.hpp"

namespace madX::Meta::glTF
{
	Source::Source(const std::filesystem::path& path)
	{
		auto logger = Core::Logger::GetInstance();

		source_ = parser_.load(path.string());

		if (source_.error())
		{
			logger->Error("[Source::Source] Error: " + path.string());
		}
		else
		{
			logger->Info("[Source::Source] Success: " + path.string());
		}

		////////////////////////////////////////////////////////////////////////////////
		
		using Element = simdjson::simdjson_result<simdjson::dom::element>;

		auto TryGetKey = [this](Element& element, const char* key) -> bool
		{
			element = source_.at_key(key);
			return simdjson::SUCCESS == element.error();
		};

		if (Element element; TryGetKey(element, "accessors"))
		{
			accessors_ = madX::Meta::glTF::GetAccessors(element.value());
		}

		if (Element element; TryGetKey(element, "buffers"))
		{
			buffers_ = madX::Meta::glTF::GetBuffers(element.value());

			const auto dir = path.parent_path();

			for (auto& buffer : buffers_)
			{
				if (buffer.uri_.has_value())
				{
					buffer.data_ = GetData(dir, buffer.uri_.value());
				}
			}
		}

		if (Element element; TryGetKey(element, "bufferViews"))
		{
			views_ = madX::Meta::glTF::GetViews(element.value());
		}

		if (Element element; TryGetKey(element, "meshes"))
		{
			meshes_ = madX::Meta::glTF::GetMeshes(element.value());
		}

		if (Element element; TryGetKey(element, "nodes"))
		{
			nodes_ = madX::Meta::glTF::GetNodes(element.value());
		}

		if (Element element; TryGetKey(element, "images"))
		{
			images_ = madX::Meta::glTF::GetImages(element.value());

			const auto dir = path.parent_path();

			for (auto& image : images_)
			{
				if (std::holds_alternative<Image::DataURI>(image.data_))
				{
					auto& data_uri = std::get<Image::DataURI>(image.data_);
					data_uri.data_ = GetData(dir, data_uri.uri_);
				}
			}
		}

		if (Element element; TryGetKey(element, "samplers"))
		{
			samplers_ = madX::Meta::glTF::GetSamplers(element.value());
		}

		if (Element element; TryGetKey(element, "textures"))
		{
			textures_ = madX::Meta::glTF::GetTextures(element.value());
		}

		if (Element element; TryGetKey(element, "materials"))
		{
			materials_ = madX::Meta::glTF::GetMaterials(element.value());
		}

		return;
	}
}