#include "_editor.hpp"

#include "layout.hpp"
#include "engine/texture.hpp"

#include "root/component/pass/gbuffer.hpp"

namespace madX::Editor::Panel
{
	std::shared_ptr<madX::Engine::Pipeline> Layout::CreateResources_Points(const madX::Engine::PipelineDesc& desc)
	{
		auto pipeline_desc = desc;

		pipeline_desc.name_ = "EditorView3D_Points";
		pipeline_desc.primitive_topology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::ShaderDesc shader_desc;

		shader_desc.name_ = "im3dVS_Points";
		shader_desc.type_ = madX::Engine::ShaderType::Vertex;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"VERTEX_SHADER");
		shader_desc.defines_.push_back(L"POINTS");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);

		shader_desc.name_ = "im3dGS_Points";
		shader_desc.type_ = madX::Engine::ShaderType::Geometry;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"GEOMETRY_SHADER");
		shader_desc.defines_.push_back(L"POINTS");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);

		shader_desc.name_ = "im3dPS_Points";
		shader_desc.type_ = madX::Engine::ShaderType::Pixel;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"PIXEL_SHADER");
		shader_desc.defines_.push_back(L"POINTS");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);

		////////////////////////////////////////////////////////////////////////////////

		return renderer_->CreatePipeline(pipeline_desc);
	}

	std::shared_ptr<madX::Engine::Pipeline> Layout::CreateResources_Lines(const madX::Engine::PipelineDesc& desc)
	{
		auto pipeline_desc = desc;

		pipeline_desc.name_ = "EditorView3D_Lines";
		pipeline_desc.primitive_topology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::ShaderDesc shader_desc;

		shader_desc.name_ = "im3dVS_Lines";
		shader_desc.type_ = madX::Engine::ShaderType::Vertex;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"VERTEX_SHADER");
		shader_desc.defines_.push_back(L"LINES");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);

		shader_desc.name_ = "im3dGS_Lines";
		shader_desc.type_ = madX::Engine::ShaderType::Geometry;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"GEOMETRY_SHADER");
		shader_desc.defines_.push_back(L"LINES");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);

		shader_desc.name_ = "im3dPS_Lines";
		shader_desc.type_ = madX::Engine::ShaderType::Pixel;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"PIXEL_SHADER");
		shader_desc.defines_.push_back(L"LINES");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);

		////////////////////////////////////////////////////////////////////////////////

		return renderer_->CreatePipeline(pipeline_desc);
	}

	std::shared_ptr<madX::Engine::Pipeline> Layout::CreateResources_Triangles(const madX::Engine::PipelineDesc& desc)
	{
		auto pipeline_desc = desc;

		pipeline_desc.name_ = "EditorView3D_Triangles";
		pipeline_desc.primitive_topology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::ShaderDesc shader_desc;

		shader_desc.name_ = "im3dVS_Triangles";
		shader_desc.type_ = madX::Engine::ShaderType::Vertex;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"VERTEX_SHADER");
		shader_desc.defines_.push_back(L"TRIANGLES");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);

		shader_desc.name_ = "im3dPS_Triangles";
		shader_desc.type_ = madX::Engine::ShaderType::Pixel;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"PIXEL_SHADER");
		shader_desc.defines_.push_back(L"TRIANGLES");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);

		////////////////////////////////////////////////////////////////////////////////

		return renderer_->CreatePipeline(pipeline_desc);
	}

	void Layout::CreatePanel(std::shared_ptr<Root::Component::Project>& project)
	{
		madX::Engine::PipelineDesc pipeline_desc;
		
		pipeline_desc.RT_count_ = 1;
		pipeline_desc.RTV_format[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		
		pipeline_desc.depth_stencil_state_.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;//D3D12_COMPARISON_FUNC_LESS;
		pipeline_desc.rasterizer_state_.CullMode = D3D12_CULL_MODE_NONE;

		////////////////////////////////////////////////////////////////////////////////
		
		auto& blend_state = pipeline_desc.blend_state_.RenderTarget[0];
		
		blend_state.BlendEnable = TRUE;
		blend_state.LogicOpEnable = FALSE;
		blend_state.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blend_state.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blend_state.BlendOp = D3D12_BLEND_OP_ADD;
		blend_state.SrcBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		blend_state.DestBlendAlpha = D3D12_BLEND_ZERO;
		blend_state.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blend_state.LogicOp = D3D12_LOGIC_OP_NOOP;
		blend_state.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	
		
		pipeline_desc.input_layout_.push_back(
		  { "POSITION_SIZE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		pipeline_desc.input_layout_.push_back(
		  { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		
		////////////////////////////////////////////////////////////////////////////////

		pipeline_desc.depth_stencil_state_.DepthEnable = TRUE;
		pipeline_desc.DSV_format = DXGI_FORMAT_D32_FLOAT;
		depth_points_ = CreateResources_Points(pipeline_desc);
		depth_lines_ = CreateResources_Lines(pipeline_desc);
		depth_triangles_ = CreateResources_Triangles(pipeline_desc);

		pipeline_desc.depth_stencil_state_.DepthEnable = FALSE;
		pipeline_desc.DSV_format = DXGI_FORMAT_UNKNOWN;
		raw_points_ = CreateResources_Points(pipeline_desc);
		raw_lines_ = CreateResources_Lines(pipeline_desc);
		raw_triangles_ = CreateResources_Triangles(pipeline_desc);

		////////////////////////////////////////////////////////////////////////////////

		for (auto& pass : frame_graph_->pass_)
		{
			pass->Create(renderer_, project);
		}

		return;
	}

	void Layout::CreateSizeDependet()
	{
		madX::Engine::TextureDesc rt_desc;
		rt_desc.name_ = "EditorView3D_RT";
		rt_desc.width_ = width_;
		rt_desc.height_ = (std::uint16_t)height_;
		rt_desc.dimension_ = madX::Engine::ResourceDimension::Texture2D;
		rt_desc.format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
		rt_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		rt_desc.bind_ = madX::Engine::ResourceBind::RenderTarget;
		rt_desc.clear_value_.color_ = { 0.16f, 0.16f, 0.16f, 1.0f };

		output_.rt_ = renderer_->CreateTexture(rt_desc, nullptr);

		madX::Engine::TextureViewDesc rtv_output_desc;
		rtv_output_desc.type_ = madX::Engine::ResourceView::RenderTarget;
		output_.rt_->CreateView(renderer_.get(), rtv_output_desc, output_.rtv_);

		madX::Engine::TextureViewDesc srv_output_desc;
		srv_output_desc.type_ = madX::Engine::ResourceView::ShaderResource;
		output_.rt_->CreateView(renderer_.get(), srv_output_desc, output_.srv_);

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::TextureDesc ds_desc;
		ds_desc.name_ = "EditorView3D_DS";
		ds_desc.width_ = width_;
		ds_desc.height_ = (std::uint16_t)height_;
		ds_desc.dimension_ = madX::Engine::ResourceDimension::Texture2D;
		ds_desc.format_ = DXGI_FORMAT_D32_FLOAT;
		ds_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		ds_desc.bind_ = madX::Engine::ResourceBind::DepthStencil;

		output_.ds_ = renderer_->CreateTexture(ds_desc, nullptr);

		madX::Engine::TextureViewDesc dsv_desc;
		dsv_desc.type_ = madX::Engine::ResourceView::DepthStencil;
		output_.ds_->CreateView(renderer_.get(), dsv_desc, output_.dsv_);
	}

	void Layout::Update(bool resize)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto delta_time = io.DeltaTime;

		const float speed = (io.KeyShift) ? 5.f : 1.0f;
		camera_.direction_delta_ = g_XMZero;
		camera_.rotation_delta_ = g_XMZero;
	
		if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && !ImGui::IsWindowCollapsed())
		{
			ImVec4 d_delta(0.0f, 0.0f, io.MouseWheel * 128.0f, 0.0f);

			if (ImGui::IsKeyDown(ImGuiKey_W)) // W (forward)
			{
				d_delta.z += speed;
			}
			if (ImGui::IsKeyDown(ImGuiKey_S)) // S (backward)
			{
				d_delta.z += -speed;
			}

			if (ImGui::IsKeyDown(ImGuiKey_D)) // D (right)
			{
				d_delta.x += speed;
			}
			if (ImGui::IsKeyDown(ImGuiKey_A)) // A (left)
			{
				d_delta.x += -speed;
			}

			if (ImGui::IsKeyDown(ImGuiKey_Q)) // Q (down)
			{
				d_delta.y += speed;
			}
			if (ImGui::IsKeyDown(ImGuiKey_E)) // E (up)
			{
				d_delta.y += -speed;
			}

			if (ImGui::IsMouseDown(1))
			{
				ImVec2 teleport;
			
				if (std::abs(io.MouseDelta.x) < (width_ * 0.5f))
				{
					teleport.x = io.MouseDelta.x;//*0.1 * ((io.KeyShift) ? 3. : 1.);
				}
			
				if (std::abs(io.MouseDelta.y) < (height_ * 0.5f))
				{
					teleport.y = io.MouseDelta.y;//*0.1 * ((io.KeyShift) ? 3. : 1.);
				}
			
				//if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
				{
					camera_.rotation_delta_ = XMVectorSet(teleport.y, teleport.x, 0.0f, 0.0f);
				}
				//else
				//{
				//	d_delta.x += -teleport.x;
				//	d_delta.y += teleport.y;
				//}
			}

			camera_.direction_delta_ = XMVectorSet(d_delta.x, d_delta.y, d_delta.z, 0.0f);
		}

		////////////////////////////////////////////////////////////////////////////////
		
		camera_.Update((float)width_, (float)height_, io.DeltaTime);

		auto& ad = Im3d::GetAppData();
		ad.m_deltaTime = io.DeltaTime;

////	if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && !ImGui::IsWindowCollapsed())
		{	
			ad.m_viewportSize = Im3d::Vec2(width_, height_);
			
			auto cam_origin = camera_.GetPosition();
			ad.m_viewOrigin = Im3d::Vec3(cam_origin.x, cam_origin.y, cam_origin.z);
			
			auto cam_direction = camera_.GetDirection();
			ad.m_viewDirection = Im3d::Vec3(cam_direction.x, cam_direction.y, cam_direction.z);

			//TODO: on camera site
			ad.m_worldUp = Im3d::Vec3(0.0f, 1.0f, 0.0f); 
			ad.m_projOrtho = false;
			////ad.m_projScaleY = false ? 2.0f / camera_.GetProjection()(1, 1) : std::tanf(camera_.GetFovRad() * 0.5f) * 2.0f;

			ImVec2 mouse = io.MousePos;
			mouse.x -= ImGui::GetCursorScreenPos().x;
			mouse.y -= ImGui::GetCursorScreenPos().y;
					
			XMVECTOR mouseNear = XMVectorSet(mouse.x, mouse.y, 1.0f, 0.0f);
			XMVECTOR unprojectedNear = XMVector3Unproject
			(
				mouseNear,
				0, 0, (float)width_, (float)height_,
				0.0f,
				1.0f,
				camera_.GetProjectionMatrix(),
				camera_.GetViewMatrix(),
				XMMatrixIdentity()
			);

			XMVECTOR mouseFar = XMVectorSet(mouse.x, mouse.y, 0.0f, 0.0f);
			XMVECTOR unprojectedFar = XMVector3Unproject
			(
				mouseFar,
				0, 0, (float)width_, (float)height_,
				0.0f,
				1.0f,
				camera_.GetProjectionMatrix(),
				camera_.GetViewMatrix(),
				XMMatrixIdentity()
			);

			XMVECTOR direction = XMVector3Normalize(XMVectorSubtract(unprojectedFar, unprojectedNear));
		
			ad.m_cursorRayOrigin = ad.m_viewOrigin;
			ad.m_cursorRayDirection = Im3d::Vec3(XMVectorGetX(direction), XMVectorGetY(direction), XMVectorGetZ(direction));
		}

		ad.m_keyDown[Im3d::Mouse_Left /*Im3d::Action_Select*/] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

		bool ctrlDown = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
		ad.m_keyDown[Im3d::Key_L /*Action_GizmoLocal*/] = ctrlDown && (GetAsyncKeyState(0x4c) & 0x8000) != 0;
		ad.m_keyDown[Im3d::Key_T /*Action_GizmoTranslation*/] = ctrlDown && (GetAsyncKeyState(0x54) & 0x8000) != 0;
		ad.m_keyDown[Im3d::Key_R /*Action_GizmoRotation*/] = ctrlDown && (GetAsyncKeyState(0x52) & 0x8000) != 0;
		ad.m_keyDown[Im3d::Key_S /*Action_GizmoScale*/] = ctrlDown && (GetAsyncKeyState(0x53) & 0x8000) != 0;

		ad.m_snapTranslation = ctrlDown ? 0.1f : 0.0f;
		ad.m_snapRotation = ctrlDown ? Im3d::Radians(30.0f) : 0.0f;
		ad.m_snapScale = ctrlDown ? 0.5f : 0.0f;

		////////////////////////////////////////////////////////////////////////////////

		//TODO: hard-fix
		if (resize)
		{
			for (auto& pass : frame_graph_->pass_)
			{
				pass->Resize(renderer_, Root::Event::Resize(width_, height_));
				//pass->Update();
			}
		}
	}

	void Layout::BeginPanel()
	{
		auto command_list = renderer_->GetCurrentCommandList();
		context_->SetCommandList(command_list);
		
		Im3d::NewFrame();
	}

	void Layout::RenderPanel(const std::shared_ptr<Root::Component::Project>& project)
	{
		ImGui::Begin("View3D test");
		ImGuiIO& io = ImGui::GetIO();

		////////////////////////////////////////////////////////////////////////////////

		{
			ImVec2 vMin = ImGui::GetWindowContentRegionMin();
			ImVec2 vMax = ImGui::GetWindowContentRegionMax();
			auto size = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);

			std::uint32_t width = size.x < 1.0f ? 1 : static_cast<uint32_t>(size.x);
			std::uint32_t height = size.y < 1.0f ? 1 : static_cast<uint32_t>(size.y);

			if (width_ != width || height_ != height)
			{
				width_ = width;
				height_ = height;

				CreateSizeDependet();
				Update(true);
			}
			else
			{
				Update(false);
			}


			const auto handle = output_.srv_.GetHandleGPU().ptr;
			ImGui::Image((ImTextureID)handle, size);
		}

		////////////////////////////////////////////////////////////////////////////////

		{	
			assert(false == frame_graph_->pass_.empty());
			auto* pass = static_cast<Root::Component::GBuffer*>(frame_graph_->pass_.front().get());

			auto& images = debug_->images_;
			images.clear();
		
			Panel::Debug::Image test1;
			test1.name_ = std::string("GBuffer::Base Color");			
			test1.width_ = pass->GetWidth();
			test1.height_ = pass->GetHeight();
			test1.view_ = &pass->color_srv_;
			images.push_back(std::move(test1));

			Panel::Debug::Image test2;
			test2.name_ = std::string("GBuffer::NormalMap");
			test2.width_ = pass->GetWidth();
			test2.height_ = pass->GetHeight();
			test2.view_ = &pass->normal_srv_;
			images.push_back(std::move(test2));

			Panel::Debug::Image test3;
			test3.name_ = std::string("GBuffer::Metalness+Roughness");
			test3.width_ = pass->GetWidth();
			test3.height_ = pass->GetHeight();
			test3.view_ = &pass->distortion_srv_;
			images.push_back(std::move(test3));

			Panel::Debug::Image test4;
			test4.name_ = std::string("GBuffer::Normal");
			test4.width_ = pass->GetWidth();
			test4.height_ = pass->GetHeight();
			test4.view_ = &pass->occlusion_srv_;
			images.push_back(std::move(test4));

			Panel::Debug::Image test5;
			test5.name_ = std::string("GBuffer::Depth");
			test5.width_ = pass->GetWidth();
			test5.height_ = pass->GetHeight();
			test5.view_ = &pass->depth_srv_;
			images.push_back(std::move(test5));
		}

		////////////////////////////////////////////////////////////////////////////////

		if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && !ImGui::IsWindowCollapsed())
		{
			auto min = ImGui::GetItemRectMin();
			auto max = ImGui::GetItemRectMax();

			if (io.MouseDown[1])
			{
				RECT rect;
				::SetRect(&rect, (int)min.x, (int)min.y, (int)max.x, (int)max.y);

				//TODO: hard-hack, ImGui_ImplWin32_Data first element is HWND
				auto handle = (HWND*)io.BackendPlatformUserData;

				::MapWindowPoints(*handle, HWND_DESKTOP, (LPPOINT)(&rect), 2);
				::ClipCursor(&rect);

				cursor_clip_ = true;
			}
			else
			{
				::ClipCursor(NULL);
				cursor_clip_ = false;
			}
		}

		ImGui::End();
	

		static Im3d::Mat4 transform((Im3d::Mat4(Im3d::Vec3(0.0, -5.0f, 0.0), Im3d::Mat3(1.0), Im3d::Vec3(1 / 50.0f))));

//		Im3d::PushLayerId("DepthAwareLayer");
//		const int gridSize = 160;
//		const float gridHalf = (float)gridSize * 0.5f;
//		Im3d::SetAlpha(1.0f);
//		Im3d::SetSize(1.f);
//		Im3d::BeginLines();
//		
//		const float alpha = 1 / gridHalf;
//		
//		Im3d::Vec3 color;
//		const Im3d::Vec3 color_hard = Im3d::Vec3(0.33f, 0.33f, 0.33f);
//		const Im3d::Vec3 color_soft = Im3d::Vec3(0.33f, 0.33f, 0.33f);
//		
//		for (int x = 0; x <= gridSize; ++x)
//		{
//			if (x % 10 == 0)
//			{
//				Im3d::SetSize(2.0f);
//				color = (x != (gridSize / 2)) ? color_hard : Im3d::Vec3(0.5f, 0.0f, 0.0f);
//			}
//			else
//			{
//				Im3d::SetSize(1.f);
//				color = color_soft;
//			}
//		
//			const auto line_alpha = 1 - std::abs((gridHalf - x) * alpha);
//			Im3d::Vertex(-gridHalf, 0.0f, (float)x - gridHalf, Im3d::Color(color, 0.0f));
//			Im3d::Vertex(0.0f, 0.0f, (float)x - gridHalf, Im3d::Color(color, line_alpha));
//			Im3d::Vertex(0.0f, 0.0f, (float)x - gridHalf, Im3d::Color(color, line_alpha));
//			Im3d::Vertex(gridHalf, 0.0f, (float)x - gridHalf, Im3d::Color(color, 0.0f));
//		}
//		
//		for (int z = 0; z <= gridSize; ++z)
//		{
//			if (z % 10 == 0)
//			{
//				Im3d::SetSize(2.0f);
//				color = (z != (gridSize / 2)) ? color_hard : Im3d::Vec3(0.0f, 0.0f, 0.5f);
//			}
//			else
//			{
//				Im3d::SetSize(1.f);
//				color = color_soft;
//			}
//		
//			const auto line_alpha = 1 - std::abs((gridHalf - z) * alpha);
//			Im3d::Vertex((float)z - gridHalf, 0.0f, -gridHalf, Im3d::Color(color, 0.0f));
//			Im3d::Vertex((float)z - gridHalf, 0.0f, 0.0, Im3d::Color(color, line_alpha));
//			Im3d::Vertex((float)z - gridHalf, 0.0f, 0.0, Im3d::Color(color, line_alpha));
//			Im3d::Vertex((float)z - gridHalf, 0.0f, gridHalf, Im3d::Color(color, 0.0f));
//		}
//		
//		Im3d::End();
//		
//		Im3d::PopLayerId();

		////////////////////////////////////////////////////////////////////////////////

		for (auto& pass : frame_graph_->pass_)
		{
			if (false == pass->IsDrawable())
				continue;

			pass->Begin(context_->GetCommandList());
			pass->Update(camera_);

			pass->Render();
		
			auto view_projection = camera_.GetViewProjection();
			project->Draw(pass->GetContext(), transform.m, *view_projection.m);

			pass->End();
		}

		//	auto inverse = camera_.GetInverseViewProjection();
		//	auto sky = (madX::Root::Component::Sky*)(pass.get());
		//	sky->SetInverseMVP(*inverse.m);

		////////////////////////////////////////////////////////////////////////////////

		Im3d::PushLayerId("AlwaysOnTopLayer");
		auto gizmo = Im3d::Gizmo("GizmoUnified", transform);
		Im3d::PopLayerId();

		End(project);
	}

	void Layout::End(const std::shared_ptr<Root::Component::Project>& project)
	{	
		Im3d::EndFrame();
		
		////////////////////////////////////////////////////////////////////////////////

		std::vector<D3D12_VIEWPORT> viewports;
		viewports.emplace_back(CD3DX12_VIEWPORT(0.0f, 0.0f, (float)width_, (float)height_));
		context_->SetViewports(viewports);
		
		auto scisors = CD3DX12_RECT(0, 0, (LONG)width_, (LONG)height_);
		context_->SetScissorRects(1, &scisors);
		
		const float blend_factors[4] = { 0.f, 0.f, 0.f, 0.f };
		context_->SetBlendFactors(blend_factors);

		auto cmd_list = context_->GetCommandList();

		const auto rtv_handle = output_.rtv_.GetHandleCPU();
		const auto dsv_handle = output_.dsv_.GetHandleCPU();
		cmd_list->OMSetRenderTargets(1, &rtv_handle, true, &dsv_handle);

		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				output_.rt_->GetResource(),
				output_.rt_->GetResourceState(),
				D3D12_RESOURCE_STATE_RENDER_TARGET
			);
			cmd_list->ResourceBarrier(1, &bb_barier);

			output_.rt_->SetResourceState(D3D12_RESOURCE_STATE_RENDER_TARGET);
		}
		float clear_color[] = { 0.16f, 0.16f, 0.16f, 1.0f }; //TODO: not seen
		cmd_list->ClearRenderTargetView(rtv_handle, clear_color, 0, NULL);

		{	
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				output_.ds_->GetResource(),
				output_.ds_->GetResourceState(),
				D3D12_RESOURCE_STATE_DEPTH_WRITE
			);
			cmd_list->ResourceBarrier(1, &bb_barier);
			output_.ds_->SetResourceState(D3D12_RESOURCE_STATE_DEPTH_WRITE);
		}

		//cmd_list->ClearDepthStencilView(dsv_handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		cmd_list->ClearDepthStencilView(dsv_handle, D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0, 0, nullptr);

		////////////////////////////////////////////////////////////////////////////////
	
		for (auto& pass : frame_graph_->pass_)
		{
			if (pass->IsDrawable())
				continue;

			pass->Begin(context_->GetCommandList());
			pass->Update(camera_);

			auto* gbuffer = (Root::Component::GBuffer*)frame_graph_->pass_.front().get();
			cmd_list->SetGraphicsRootDescriptorTable(1, gbuffer->GetDepthSRV());

			const auto hdr_handle = project->GetHDR().GetHandleGPU();
			cmd_list->SetGraphicsRootDescriptorTable(2, hdr_handle);

			pass->Render();
			pass->End();
		}
	
		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::BufferDesc buffer_desc;
		buffer_desc.name_ = "cbContextData";
		buffer_desc.count_ = 1;
		buffer_desc.stride_ = sizeof(Constants);
		buffer_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		buffer_desc.bind_ = madX::Engine::ResourceBind::UniformBuffer;
		
		constants_.view_projection_ = camera_.GetViewProjection();
		constants_.viewport_.x = (float)width_;
		constants_.viewport_.y = (float)height_;
		
		auto constants = renderer_->CreateBuffer(buffer_desc);
		
		Constants* cb_constants = (Constants*)constants->GetAddressCPU();
		std::memcpy(cb_constants, &constants_, sizeof(Constants));
		
		for (Im3d::U32 index = 0, count = Im3d::GetDrawListCount(); index < count; ++index)
		{
			auto& drawList = Im3d::GetDrawLists()[index];		
			
			auto depth_enable = drawList.m_layerId == Im3d::MakeId("DepthAwareLayer");
		
			madX::Engine::BufferDesc vb_desc;
			vb_desc.name_ = "im3D_VB::" + std::to_string(index);
			vb_desc.count_ = drawList.m_vertexCount;
			vb_desc.stride_ = sizeof(Im3d::VertexData);
			vb_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
			vb_desc.bind_ = madX::Engine::ResourceBind::VertexBuffer;
			auto vb = renderer_->CreateBuffer(vb_desc);
		
			auto* vtx_dst = (Im3d::VertexData*)vb->GetAddressCPU();
			std::memcpy(vtx_dst, drawList.m_vertexData, drawList.m_vertexCount * sizeof(Im3d::VertexData));
		
			switch (drawList.m_primType)
			{
			case Im3d::DrawPrimitive_Points:
			{
				context_->SetPipeline(depth_enable ? depth_points_ : raw_points_);
				context_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
				break;
			}
			case Im3d::DrawPrimitive_Lines:
			{
				context_->SetPipeline(depth_enable ? depth_lines_ : raw_lines_);
				context_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
				break;
			}
			case Im3d::DrawPrimitive_Triangles:
			{
				context_->SetPipeline(depth_enable ? depth_triangles_ : raw_triangles_);
				context_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				break;
			}
			default:
				assert(false);
				return;
			};
		
			auto test = std::make_pair<std::shared_ptr<madX::Engine::Buffer>, std::size_t>(std::move(vb), (std::size_t)0);
			std::vector<madX::Engine::VertexStream> test2 = { test };
			context_->SetVertexBuffer(0, test2);
		
			context_->CommitShaderResources();
			context_->CommitConstantBuffers(constants);
		
			madX::Engine::DrawAttributes draw;
			draw.vertices_ = drawList.m_vertexCount;
			context_->Draw(draw);
		}	

		////////////////////////////////////////////////////////////////////////////////

		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				output_.rt_->GetResource(),
				output_.rt_->GetResourceState(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);
		
			cmd_list->ResourceBarrier(1, &bb_barier);
			output_.rt_->SetResourceState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}

		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				output_.ds_->GetResource(),
				output_.ds_->GetResourceState(),
				D3D12_RESOURCE_STATE_COMMON 
			);
		
			cmd_list->ResourceBarrier(1, &bb_barier);
			output_.ds_->SetResourceState(D3D12_RESOURCE_STATE_COMMON);
		}
	}
}