#include "_root.hpp"

//#include "imgui/imgui.h"
//#include "imgui/imgui_internal.h"

namespace madX::Root::Component
{
	void Project::CreateBuffers(const std::shared_ptr<madX::Engine::Renderer>& renderer)
	{
		assert(source_);

		auto& accessors = source_->GetAccessors();
		auto& buffers = source_->GetBuffers();
		auto& views = source_->GetViews();
		auto& meshes = source_->GetMeshes();

		auto& buffer = buffers[0];

		auto& mesh = meshes[0];
		assert(mesh.primitives_.size() == 1);

		auto& suzanne = mesh.primitives_[0];

		////////////////////////////////////////////////////////////////////////////////

		assert(false == suzanne.attributes_.empty());
		assert(suzanne.attributes_.size() == 4);
		auto& position = suzanne.attributes_[1];
		assert(position.id_ == "POSITION");
		auto& accessor_vertices = accessors.at(position.accessor_);
		assert(accessor_vertices.view_.has_value());
		auto& view_vertices = views[accessor_vertices.view_.value()];

		madX::Engine::BufferDesc vb_desc;
		vb_desc.name_ = "SuzanneVB";
		vb_desc.size_ = view_vertices.size_;
		vb_desc.stride_ = accessor_vertices.component_ * accessor_vertices.element_;
		vb_desc.usage_ = madX::Engine::ResourceUsage::Static;
		vb_desc.bind_ = madX::Engine::ResourceBind::VertexBuffer;

		auto offset_vertices = view_vertices.offset_;
		auto length_vertices = view_vertices.size_;
		auto byte_offset_vertices = accessor_vertices.offset_;
		offset_vertices += byte_offset_vertices;
		length_vertices -= byte_offset_vertices;

		//madX::Engine::BufferData vb_data;
		//vb_data.data_ = &buffer.data_[offset_vertices];
		//vb_data.size_ = vb_desc.size_;
		vb_desc.data_ = &buffer.data_[offset_vertices];

		vb_ = renderer->CreateBuffer(vb_desc);

		////////////////////////////////////////////////////////////////////////////////

		assert(suzanne.indices_.has_value());
		auto& accessor_indices = accessors.at(suzanne.indices_.value());
		assert(accessor_indices.view_.has_value());
		auto& view_indices = views[accessor_indices.view_.value()];

		//TODO: meta holder
		indices_count_ = accessor_indices.count_;

		//TODO: watch out! some exporters use 1-byte idices
		assert(accessor_indices.component_ != 1);
		madX::Engine::BufferDesc ib_desc;
		ib_desc.name_ = "SuzanneIB";
		ib_desc.size_ = view_indices.size_;
		ib_desc.stride_ = accessor_indices.component_ * accessor_indices.element_;
		ib_desc.usage_ = madX::Engine::ResourceUsage::Static;
		ib_desc.bind_ = madX::Engine::ResourceBind::IndexBuffer;

		auto offset_indices = view_indices.offset_;
		auto length_indices = view_indices.size_;
		;
		auto byte_offset_indices = accessor_indices.offset_;
		offset_indices += byte_offset_indices;
		length_indices -= byte_offset_indices;

		//madX::Engine::BufferData ib_data;
		//ib_data.data_ = &buffer.data_[offset_indices];
		//ib_data.size_ = ib_desc.size_;
		ib_desc.data_ = &buffer.data_[offset_indices];

		ib_ = renderer->CreateBuffer(ib_desc);
	}

	Project::Project(const std::shared_ptr<madX::Engine::Renderer>& renderer)
	{
		//TODO: improve me
		source_ = std::make_unique<Meta::glTF::Source>("assets\\suzanne\\suzanne.gltf");
		
		CreateBuffers(renderer);
	}

	void Project::Destroy()
	{
		vb_.reset();
		ib_.reset();

		source_.reset();
		//parser_.~parser();
		//path_.~path();
	}

	void Project::Update(const Event::Frame::Update update)
	{
	//	float angle = static_cast<float>(update.total_ * 90.0);
	//	const XMVECTOR rotationAxis = XMVectorSet(0, 1, 0, 0);
	//	auto model = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));
	//	
	//	const XMVECTOR eyePosition = XMVectorSet(0, 0, -5, 1);
	//	const XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
	//	const XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
	//	auto view = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
	//	
	//	float aspectRatio = (float)(update.width_) / (float)(update.height_);
	//	auto projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), aspectRatio, 0.1f, 100.0f);

		ImGuiContext* context = ImGui::GetCurrentContext();

		if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && !ImGui::IsWindowCollapsed())
		{	
			if (context->IO.KeysDown[0x57]) //W
			{
				camera_.direction_delta_ = DirectX::XMVectorSetZ(camera_.direction_delta_, +1.0f);
			}
			else if (context->IO.KeysDown[0x53]) //S
			{
				camera_.direction_delta_ = DirectX::XMVectorSetZ(camera_.direction_delta_, -1.0f);
			}
			else
			{
				camera_.direction_delta_ = DirectX::XMVectorSetZ(camera_.direction_delta_, 0.0f);
			}

			if (context->IO.KeysDown[0x41]) //A
			{
				camera_.direction_delta_ = DirectX::XMVectorSetX(camera_.direction_delta_, -1.0f);
			}
			else if (context->IO.KeysDown[0x44]) //D
			{
				camera_.direction_delta_ = DirectX::XMVectorSetX(camera_.direction_delta_, +1.0f);
			}
			else
			{
				camera_.direction_delta_ = DirectX::XMVectorSetX(camera_.direction_delta_, 0.0f);
			}

			if (context->IO.MouseDown[1])
			{
				DirectX::XMVECTOR delta = DirectX::XMVectorSet(context->IO.MouseDelta.y, context->IO.MouseDelta.x, 0.0f, 0.0f);
				camera_.rotation_delta_ = DirectX::XMVectorAdd(camera_.rotation_delta_, delta);
			}
		}
		else
		{
			camera_.direction_delta_ = DirectX::XMVectorSetZ(camera_.direction_delta_, 0.0f);
			camera_.direction_delta_ = DirectX::XMVectorSetX(camera_.direction_delta_, 0.0f);
		}

		float aspect_ratio = (float)(update.width_) / (float)(update.height_);
		camera_.Update(context->IO.DeltaTime, aspect_ratio);

		auto model = XMMatrixIdentity();
		auto view = camera_.GetViewMatrix();
		auto projection = camera_.GetProjectionMatrix();

		mvp_ = XMMatrixMultiply(model, view);
		mvp_ = XMMatrixMultiply(mvp_, projection);
	}

	void Project::Draw(std::unique_ptr<madX::Engine::Context>& context)
	{
		auto renderer = context->GetRenderer();

		madX::Engine::BufferDesc buffer_desc;
		buffer_desc.name_ = "Constants";
		buffer_desc.size_ = sizeof(XMMATRIX);
		buffer_desc.stride_ = sizeof(XMMATRIX);
		buffer_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		buffer_desc.bind_ = madX::Engine::ResourceBind::UniformBuffer;

		auto vs_constants = renderer->CreateBuffer(buffer_desc);

		{
			XMMATRIX* cb_constants = (XMMATRIX*)vs_constants->GetAddressCPU();
			*cb_constants = mvp_;
		}

		////////////////////////////////////////////////////////////////////////////////

		std::pair<std::shared_ptr<madX::Engine::Buffer>, std::size_t> test = { vb_, 0 };
		std::vector<madX::Engine::VertexStream> test2 = { test };
		context->SetVertexBuffer(0, test2);
		context->SetIndexBuffer(ib_, 0u);	
		context->CommitShaderResources();
		context->CommitConstantBuffers(vs_constants);
			
		//TODO: fixed
		madX::Engine::DrawIndexedAttributes draw;
		draw.indices_ = indices_count_;
		context->DrawIndexed(draw);
	}
}