#include "_root.hpp"

//	https://docs.microsoft.com/en-us/windows/win32/wic/-wic-about-windows-imaging-codec
//	https://github.com/microsoft/DirectXTK12/wiki/WICTextureLoader

namespace madX::Root::Component
{
	std::shared_ptr<madX::Engine::Texture> Project::CreateTexture(const std::shared_ptr<madX::Engine::Renderer>& renderer, Meta::glTF::Index index)
	{
		auto logger = Core::Logger::GetInstance();

		auto device = renderer->GetDevice();

		const auto& images = source_->GetImages();
		const auto& textures = source_->GetTextures();

		const auto& texture = textures[index];
		assert(texture.source_.has_value());
		const auto texture_index = texture.source_.value();
		
		const auto& image = images[texture_index];

		if (auto uri = std::get_if<Meta::glTF::Image::DataURI>(&image.data_))
		{
			logger->Info("Loading image: " + uri->uri_);

			if (texture_.contains(index))
			{
				logger->Error(" * Failed (texture already present): " + std::to_string(index));
				return {};
			}

			DirectX::ScratchImage scratch;

			auto result = LoadFromWICMemory
			(
				uri->data_.data(),
				uri->data_.size(),
				WIC_FLAGS_NONE,
				nullptr,
				scratch
			);

			if (FAILED(result))
			{
				logger->Error(" * Failed (RESULT): " + std::to_string(result));
				return {};
			}

			const auto& meta_data = scratch.GetMetadata();
			assert(meta_data.mipLevels == 1);

			madX::Engine::TextureDesc texture_desc;
			texture_desc.name_ = "Texture::glTF" + std::to_string(index);
			texture_desc.width_ = meta_data.width;
			texture_desc.height_ = meta_data.height;;
			texture_desc.dimension_ = madX::Engine::ResourceDimension::Texture2D;
			texture_desc.format_ = meta_data.format;
			texture_desc.usage_ = madX::Engine::ResourceUsage::Static;
			texture_desc.bind_ = madX::Engine::ResourceBind::ShaderResource;

			madX::Engine::TextureResourceData texture_mip[] = { { scratch.GetPixels(), 0, 0 } };
			madX::Engine::TextureData texture_data(texture_mip, _countof(texture_mip));

			return renderer->CreateTexture(texture_desc, &texture_data);
		}


		return std::shared_ptr<madX::Engine::Texture>();
	}

	void Project::BuildBuffers(const std::shared_ptr<madX::Engine::Renderer>& renderer)
	{
		assert(source_);

		const auto& accessors = source_->GetAccessors();
		const auto& buffers = source_->GetBuffers();
		const auto& views = source_->GetViews();
		const auto& meshes = source_->GetMeshes();

		assert(buffers.size() == 1);
		const auto& buffer = buffers[0];

		for (const auto& mesh : meshes)
		{
			for (const auto& model : mesh.primitives_)
			{
				//TODO: loop on all attributes -> keep position.id_ in the storage
				assert(false == model.attributes_.empty());
				for (const auto& attribute : model.attributes_)
				{
					const auto id = attribute.second;

					const auto& accessor_vertices = accessors.at(id);
					assert(accessor_vertices.view_.has_value());
					const auto& view_vertices = views[accessor_vertices.view_.value()];

					madX::Engine::BufferDesc vb_desc;
					vb_desc.name_ = "ModelVB::ID" + std::to_string(id) + "::" + attribute.first;

					vb_desc.count_ = accessor_vertices.count_;
					vb_desc.stride_ = accessor_vertices.component_ * accessor_vertices.element_;
					vb_desc.usage_ = madX::Engine::ResourceUsage::Static;
					vb_desc.bind_ = madX::Engine::ResourceBind::VertexBuffer;

					auto offset_vertices = view_vertices.offset_;
					//auto length_vertices = view_vertices.size_;
					const auto byte_offset_vertices = accessor_vertices.offset_;
					offset_vertices += byte_offset_vertices;
					//length_vertices -= byte_offset_vertices;

					vb_desc.data_ = &buffer.data_[offset_vertices];
					vb_[id] = renderer->CreateBuffer(vb_desc);		
				}

				////////////////////////////////////////////////////////////////////////////////

				//TODO: if no indices -> continue
				assert(model.indices_.has_value());
				const auto id = model.indices_.value();

				const auto& accessor_indices = accessors.at(id);
				assert(accessor_indices.component_ != 1); //TODO: watch out! some exporters use 1-byte idices
				assert(accessor_indices.view_.has_value());
				const auto& view_indices = views[accessor_indices.view_.value()];

				madX::Engine::BufferDesc ib_desc;
				ib_desc.name_ = "ModelIB::ID" + std::to_string(id);
				ib_desc.count_ = accessor_indices.count_;
				ib_desc.stride_ = accessor_indices.component_ * accessor_indices.element_;
				ib_desc.usage_ = madX::Engine::ResourceUsage::Static;
				ib_desc.bind_ = madX::Engine::ResourceBind::IndexBuffer;

				auto offset_indices = view_indices.offset_;
				//auto length_indices = view_indices.size_;
				const auto byte_offset_indices = accessor_indices.offset_;
				offset_indices += byte_offset_indices;
				//length_indices -= byte_offset_indices;

				ib_desc.data_ = &buffer.data_[offset_indices];
				ib_[id] = renderer->CreateBuffer(ib_desc);
			}
		}

		return;
	}

	void Project::BuildTextures(const std::shared_ptr<madX::Engine::Renderer>& renderer)
	{
		auto logger = Core::Logger::GetInstance();

		const auto& materials = source_->GetMaterials();

		for (const auto& node : source_->GetNodes())
		{
			if (false == node.mesh_.has_value())
			{
				continue;
			}

			const auto& mesh = source_->GetMeshes()[node.mesh_.value()];
			for (const auto& model : mesh.primitives_)
			{
				assert(model.material_.has_value());
				const auto material_index = model.material_.value();
				const auto& material = materials.at(material_index);

				assert(material.pbr_.has_value());
				const auto& material_pbr = material.pbr_.value();

				assert(material_pbr.base_texture_.has_value());
				auto base_index = material_pbr.base_texture_.value().index_;

				auto normal_index = material.normal_.has_value()
					? material.normal_.value().index_
					: Meta::glTF::InvalidIndex;

				auto distortion_index = material_pbr.mr_texture_.has_value()
					? material_pbr.mr_texture_.value().index_
					: Meta::glTF::InvalidIndex;

				if (texture_.contains(base_index))
				{
					continue;
				}

				Texture base_tex;
				
				base_tex.tex_ = CreateTexture(renderer, base_index);
				assert(base_tex.tex_);
				
				madX::Engine::TextureViewDesc base_view_desc;
				base_view_desc.type_ = madX::Engine::ResourceView::ShaderResource;

				//TODO
				Engine::NativeViewDesc null_desc = base_tex.tex_->CreateView(renderer.get(), base_view_desc, base_tex.srv_);
				
				texture_[base_index] = std::move(base_tex);

			//	if (false == texture_.contains(base_index))
			//	{
			//		Texture base_tex;
			//	
			//		base_tex.tex_ = CreateTexture(renderer, base_index);
			//		assert(base_tex.tex_);
			//		
			//		madX::Engine::TextureViewDesc base_view_desc;
			//		base_view_desc.type_ = madX::Engine::ResourceView::ShaderResource;
			//		base_tex.null_desc_ = base_tex.tex_->CreateView(renderer.get(), base_view_desc, base_tex.srv_);
			//	
			//		texture_[base_index] = std::move(base_tex);
			//	}
			//	else
			//	{
			//		continue;
			//		madX::Engine::TextureView srv;
			//	
			//		madX::Engine::TextureViewDesc base_view_desc;
			//		base_view_desc.type_ = madX::Engine::ResourceView::ShaderResource;
			//		texture_[base_index].tex_->CreateView(renderer.get(), base_view_desc, srv);
			//	}
			//	
			//	Engine::NativeViewDesc null_desc = texture_[base_index].null_desc_;

				////////////////////////////////////////////////////////////////////////////////

				if (material.normal_.has_value())
				{
					//auto normal_index = material.normal_.value().index_;
				
					if (false == texture_.contains(normal_index))
					{
						Texture normal_tex;
						normal_tex.tex_ = CreateTexture(renderer, normal_index);
						assert(normal_tex.tex_);
				
						madX::Engine::TextureViewDesc normal_view_desc;
						normal_view_desc.type_ = madX::Engine::ResourceView::ShaderResource;
						normal_tex.tex_->CreateView(renderer.get(), normal_view_desc, normal_tex.srv_);
				
						texture_[normal_index] = std::move(normal_tex);
					}
					else
					{
						madX::Engine::TextureView srv;
					
						madX::Engine::TextureViewDesc normal_view_desc;
						normal_view_desc.type_ = madX::Engine::ResourceView::ShaderResource;
						texture_[normal_index].tex_->CreateView(renderer.get(), normal_view_desc, srv);
					}
				}
				else
				{
					Engine::Texture::CreateNullBind(renderer.get(), null_desc);
				}

				////////////////////////////////////////////////////////////////////////////////

				if (material_pbr.mr_texture_.has_value())
				{
					//auto distortion_index = material_pbr.mr_texture_.value().index_;
				
					if (false == texture_.contains(distortion_index))
					{
						Texture distortion_tex;
						distortion_tex.tex_ = CreateTexture(renderer, distortion_index);
						assert(distortion_tex.tex_);
				
						madX::Engine::TextureViewDesc distortion_view_desc;
						distortion_view_desc.type_ = madX::Engine::ResourceView::ShaderResource;
						distortion_tex.tex_->CreateView(renderer.get(), distortion_view_desc, distortion_tex.srv_);
				
						texture_[distortion_index] = std::move(distortion_tex);
					}
					else
					{
						madX::Engine::TextureView srv;
					
						madX::Engine::TextureViewDesc distortion_view_desc;
						distortion_view_desc.type_ = madX::Engine::ResourceView::ShaderResource;
						texture_[distortion_index].tex_->CreateView(renderer.get(), distortion_view_desc, srv);
					}
				}
				else
				{
					Engine::Texture::CreateNullBind(renderer.get(), null_desc);
				}

				////////////////////////////////////////////////////////////////////////////////
			}
		}

		return;
	}

	Project::Project(const std::shared_ptr<madX::Engine::Renderer>& renderer)
	{
		return;
		//source_ = std::make_unique<Meta::glTF::Source>("assets\\SciFiHelmet\\SciFiHelmet.gltf");
		source_ = std::make_unique<Meta::glTF::Source>("assets\\sponza\\sponza.gltf");
	
		BuildBuffers(renderer);
		BuildTextures(renderer);

		auto logger = Core::Logger::GetInstance();

		DirectX::ScratchImage scratch;
		//HRESULT result = LoadFromHDRFile(L"hdr\\EtniesPark_Center\\Etnies_Park_Center_3k.hdr", nullptr, scratch);
		HRESULT result = LoadFromHDRFile(L"hdr\\Ditch_River\\Ditch-River_2k.hdr", nullptr, scratch);

		if (SUCCEEDED(result))
		{
			const auto& meta_data = scratch.GetMetadata();
			assert(meta_data.mipLevels == 1);

			madX::Engine::TextureDesc texture_desc;
			texture_desc.name_ = "TextureHDR";
			texture_desc.width_ = meta_data.width;
			texture_desc.height_ = meta_data.height;
			
			texture_desc.dimension_ = madX::Engine::ResourceDimension::Texture2D;
			texture_desc.format_ = meta_data.format;
			texture_desc.usage_ = madX::Engine::ResourceUsage::Static;
			texture_desc.bind_ = madX::Engine::ResourceBind::ShaderResource;

			madX::Engine::TextureResourceData texture_mip[] = { { scratch.GetPixels(), 0, 0 } };
			madX::Engine::TextureData texture_data(texture_mip, _countof(texture_mip));

			madX::Engine::TextureViewDesc view_desc;
			view_desc.type_ = madX::Engine::ResourceView::ShaderResource;

			hdr_.tex_ = renderer->CreateTexture(texture_desc, &texture_data);
			hdr_.tex_->CreateView(renderer.get(), view_desc, hdr_.srv_);
		}
		else
		{
			logger->Error("Loading HDR failed (RESULT): " + std::to_string(result));
		}
	}

	void Project::Destroy()
	{
		vb_.clear(); //vb_.reset();
		ib_.clear(); //ib_.reset();

		source_.reset();
		//parser_.~parser();
		//path_.~path();
	}

	void Project::Update(const Event::Frame::Update update)
	{
	}

	void Project::Draw(std::unique_ptr<madX::Engine::Context>& context, const float* world, const float* view_projection)
	{
		const auto& materials = source_->GetMaterials();
		
		auto renderer = context->GetRenderer();

		//TODO: Constants
		madX::Engine::BufferDesc buffer_desc;
		buffer_desc.name_ = "Constants";
		buffer_desc.count_ = 1;
		buffer_desc.stride_ = sizeof(XMFLOAT4X4) * 3;
		buffer_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		buffer_desc.bind_ = madX::Engine::ResourceBind::UniformBuffer;

		auto vs_constants = renderer->CreateBuffer(buffer_desc);

		//if (world != nullptr && view_projection != nullptr)
		{
			XMMATRIX m_world(world);
			XMMATRIX m_inv_world = XMMatrixInverse(nullptr, m_world);

			XMFLOAT4X4 inv_world;
			XMStoreFloat4x4(&inv_world, m_inv_world);

			XMFLOAT4X4* cb_constants = (XMFLOAT4X4*)vs_constants->GetAddressCPU();
			std::memcpy(cb_constants++, world, sizeof(XMFLOAT4X4));
			std::memcpy(cb_constants++, inv_world.m, sizeof(XMFLOAT4X4));
			std::memcpy(cb_constants++, view_projection, sizeof(XMFLOAT4X4));
		}

		////////////////////////////////////////////////////////////////////////////////

		auto pipeline = context->GetPipeline();
		auto command_list = context->GetCommandList();

		for (const auto& node : source_->GetNodes())
		{
			if (false == node.mesh_.has_value())
			{
				continue;
			}

			const auto& mesh = source_->GetMeshes()[node.mesh_.value()];
			for (const auto& model : mesh.primitives_)
			{			
				const auto attributes_size = model.attributes_.size();
				assert(attributes_size > 2);
				assert(attributes_size < 5);

				//sponza
				const auto position_index = model.attributes_.at("POSITION");
				auto position = vb_[position_index];

				const auto texture_index = model.attributes_.at("TEXCOORD_0");
				auto texture = vb_[texture_index];

				const auto normal_index = model.attributes_.at("NORMAL");
				auto normal = vb_[normal_index];

				std::vector<madX::Engine::VertexStream> vb_stream =
				{
					std::make_pair<std::shared_ptr<madX::Engine::Buffer>, std::size_t>(std::move(position), 0),
					std::make_pair<std::shared_ptr<madX::Engine::Buffer>, std::size_t>(std::move(texture), 0),
					std::make_pair<std::shared_ptr<madX::Engine::Buffer>, std::size_t>(std::move(normal), 0)
				};

				//TODO: texture swapping to fit into bindless 
				if (attributes_size == 4)
				{
					const auto tangent_index = model.attributes_.at("TANGENT");
					auto tangent = vb_[tangent_index];

					vb_stream.emplace_back(std::make_pair<std::shared_ptr<madX::Engine::Buffer>, std::size_t>(std::move(tangent), 0));
				}

				auto& indices = ib_[model.indices_.value()];
				
			//	std::vector<madX::Engine::VertexStream> vb_stream =
			//	{
			//		std::make_pair<std::shared_ptr<madX::Engine::Buffer>, std::size_t>(std::move(position), 0),
			//		std::make_pair<std::shared_ptr<madX::Engine::Buffer>, std::size_t>(std::move(normal), 0),
			//		std::make_pair<std::shared_ptr<madX::Engine::Buffer>, std::size_t>(std::move(tangent), 0),
			//		std::make_pair<std::shared_ptr<madX::Engine::Buffer>, std::size_t>(std::move(texture), 0),
			//	
			//	};
				context->SetVertexBuffer(0, vb_stream);
				context->SetIndexBuffer(indices, 0u);
				
				context->CommitShaderResources();

				assert(model.material_.has_value());
				const auto& material = materials.at(model.material_.value());
				
				assert(material.pbr_.has_value());
				const auto& pbr = material.pbr_.value();
				
				assert(pbr.base_texture_.has_value());
				const auto& base_tex_index = pbr.base_texture_.value();

				const auto& base_tex = texture_.at(base_tex_index.index_);
							
				const auto handle = base_tex.srv_.GetHandleGPU();
				command_list->SetGraphicsRootDescriptorTable(1, handle);

				context->CommitConstantBuffers(vs_constants);
							
				madX::Engine::DrawIndexedAttributes draw;
				draw.indices_ = indices->GetBufferCount();
				context->DrawIndexed(draw);
			}
		}
	}
}



//	assert(vb_.size() == ib_.size());
//
//	auto vb = vb_.begin();
//	auto ib = ib_.begin();
//
//	for (auto index = 0; index < vb_.size(); ++index)
//	{
//		std::pair<std::shared_ptr<madX::Engine::Buffer>, std::size_t> vb_dummy = { vb->second, 0 };
//		std::vector<madX::Engine::VertexStream> vb_stream = { vb_dummy };
//		context->SetVertexBuffer(0, vb_stream);
//		context->SetIndexBuffer(ib->second, 0u);
//		context->CommitShaderResources();
//		context->CommitConstantBuffers(vs_constants);
//
//		//TODO: fixed
//		madX::Engine::DrawIndexedAttributes draw;
//		draw.indices_ = ib->second->GetBufferSize() / ib->second->GetBufferStride();
//		context->DrawIndexed(draw);
//
//		vb++;
//		ib++;
//	}



//		//	auto device = renderer->GetDevice();
//		//	
//		//	const auto& images = source_->GetImages();
//		//	const auto& textures = source_->GetTextures();
//	
//		//		for (const auto& texture : textures)
//		for (const auto& index : order_)
//		{
//			//	if (!texture.source_.has_value())
//			//	{
//			//		assert(false);
//			//	}
//	
//		//	if (index == Meta::glTF::InvalidIndex)
//		//	{
//		//		continue;
//		//	}
//	
//	
//			//	madX::Engine::TextureDesc texture_desc;
//			//	texture_desc.name_ = "Texture";
//			//	texture_desc.width_ = meta_data.width;
//			//	texture_desc.height_ = meta_data.height;;
//			//	texture_desc.dimension_ = madX::Engine::ResourceDimension::Texture2D;
//			//	texture_desc.format_ = meta_data.format;
//			//	texture_desc.usage_ = madX::Engine::ResourceUsage::Static;
//			//	texture_desc.bind_ = madX::Engine::ResourceBind::ShaderResource;
//			//	
//			//	madX::Engine::TextureResourceData texture_mip[] = { { scratch.GetPixels(), 0, 0 } };
//			//	madX::Engine::TextureData texture_data(texture_mip, _countof(texture_mip));
//			//	
//			//	madX::Engine::TextureViewDesc view_desc;
//			//	view_desc.type_ = madX::Engine::ResourceView::ShaderResource;
//			//	
//			//	Texture texture;
//			//	texture.tex_ = renderer->CreateTexture(texture_desc, &texture_data);
//			//	texture.tex_->CreateView(renderer.get(), view_desc, texture.srv_);
//			//	
//			//	texture_[index] = std::move(texture);
//			//	//texture_.emplace_back(std::move(texture));
//			//	//texture_.emplace_back(std::move(texture));
//		}



//	void Project::BuildTexturesOrder()
//	{
//		const auto& materials = source_->GetMaterials();
//	
//		for (const auto& node : source_->GetNodes())
//		{
//			if (false == node.mesh_.has_value())
//			{
//				continue;
//			}
//	
//			const auto& mesh = source_->GetMeshes()[node.mesh_.value()];
//			for (const auto& model : mesh.primitives_)
//			{
//				assert(model.material_.has_value());
//				const auto& material = materials.at(model.material_.value());
//	
//				assert(material.pbr_.has_value());
//				const auto& pbr = material.pbr_.value();
//	
//				assert(pbr.base_texture_.has_value());
//				const auto& base_texture = pbr.base_texture_.value();
//	
//	
//				Order order;
//				order.base_ = base_texture.index_;
//				
//				//assert(material.normal_.has_value());
//				order.normal_ = material.normal_.has_value()
//					? material.normal_.value().index_
//					: Meta::glTF::InvalidIndex;
//				
//				order.distortion_ = pbr.mr_texture_.has_value()
//					? pbr.mr_texture_.value().index_
//					: Meta::glTF::InvalidIndex;
//				
//			//	order_.push_back(order.base_);
//			//	order_.push_back(order.normal_);
//			//	order_.push_back(order.distortion_);
//				order_.emplace_back(std::move(order));
//			}
//		}
//	}