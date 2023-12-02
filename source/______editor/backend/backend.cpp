#include "_editor.hpp"

#include "backend.hpp"

#include "engine/context.hpp"

#include "imgui.h"
#include "imgui/backends/imgui_impl_win32.h"

#include "im3d.h"
#include "im3d_math.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

//https://github.com/microsoft/terminal/blob/main/src/cascadia/WindowsTerminal/IslandWindow.cpp
//https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/Samples/Desktop/D3D12Fullscreen/src/D3D12Fullscreen.cpp

namespace madX::Editor
{
	struct BackendData
	{
		std::shared_ptr<madX::Engine::Renderer> renderer_;
		std::shared_ptr<madX::Engine::Pipeline> pipeline_imgui_;
		std::shared_ptr<madX::Engine::Pipeline> pipeline_raw_;
		std::unique_ptr<madX::Engine::Context> context_;
		std::shared_ptr<madX::Engine::SwapChain> swapchain_;

		std::shared_ptr<madX::Engine::Texture> fonts_;
		madX::Engine::TextureView fonts_srv_;
	};

	struct ViewportData
	{
		//TODO: no ptr
		std::unique_ptr<madX::Engine::Commands> commands_;
		std::unique_ptr<madX::Engine::Context> context_;
		std::shared_ptr<madX::Engine::SwapChain> swapchain_;

		madX::Engine::Frame frame_ = 0;
	};

	////////////////////////////////////////////////////////////////////////////////

	static BackendData* GetBackendData()
	{
		return ImGui::GetCurrentContext() ? (BackendData*)ImGui::GetIO().BackendRendererUserData : NULL;
	}

	void RenderData(ImDrawData* draw_data, ID3D12GraphicsCommandList* command_list)
	{
		assert(draw_data != nullptr);

		if (draw_data->DisplaySize.x < 1.0f || draw_data->DisplaySize.y < 1.0f)
		{
			return;
		}

		BackendData* bd = GetBackendData();
		auto context = bd->context_.get();

		////////////////////////////////////////////////////////////////////////////////

		context->SetCommandList(command_list);
		auto renderer = bd->renderer_.get();

		madX::Engine::BufferDesc vb_desc;
		vb_desc.name_ = "imgui_VB";
		vb_desc.count_ = draw_data->TotalVtxCount;
		vb_desc.stride_ = sizeof(ImDrawVert);
		vb_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		vb_desc.bind_ = madX::Engine::ResourceBind::VertexBuffer;
		auto vb = renderer->CreateBuffer(vb_desc);

		madX::Engine::BufferDesc ib_desc;
		ib_desc.name_ = "imgui_IB";
		ib_desc.count_ = draw_data->TotalIdxCount;
		ib_desc.stride_ = sizeof(ImDrawIdx);
		ib_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		ib_desc.bind_ = madX::Engine::ResourceBind::IndexBuffer;
		auto ib = renderer->CreateBuffer(ib_desc);

		////////////////////////////////////////////////////////////////////////////////

		{
			auto* vtx_dst = (ImDrawVert*)vb->GetAddressCPU();
			auto* idx_dst = (ImDrawIdx*)ib->GetAddressCPU();

			for (int list_index = 0; list_index < draw_data->CmdListsCount; ++list_index)
			{
				const auto cmd_list = draw_data->CmdLists[list_index];

				assert(cmd_list->VtxBuffer.Data != nullptr);
				assert(cmd_list->VtxBuffer.Size > 0);

				std::memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));

				assert(cmd_list->IdxBuffer.Data != nullptr);
				assert(cmd_list->IdxBuffer.Size > 0);

				std::memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

				vtx_dst += cmd_list->VtxBuffer.Size;
				idx_dst += cmd_list->IdxBuffer.Size;
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::BufferDesc buffer_desc;
		buffer_desc.name_ = "Constants";
		buffer_desc.count_ = 1;
		buffer_desc.stride_ = sizeof(XMFLOAT4X4);
		buffer_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		buffer_desc.bind_ = madX::Engine::ResourceBind::UniformBuffer;

		auto vs_constants = renderer->CreateBuffer(buffer_desc);

		{
			XMFLOAT4X4* cb_constants = (XMFLOAT4X4*)vs_constants->GetAddressCPU();

			float L = draw_data->DisplayPos.x;
			float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
			float T = draw_data->DisplayPos.y;
			float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

			*cb_constants = XMFLOAT4X4(2.0f / (R - L), 0.0f, 0.0f, 0.0f, 0.0f, 2.0f / (T - B), 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f,
			  (R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f);
		}

		////////////////////////////////////////////////////////////////////////////////

		//	const float blend_factors[4] = { 0.f, 0.f, 0.f, 0.f };
		//	context->SetBlendFactors(blend_factors);


		std::vector<D3D12_VIEWPORT> viewports;
		viewports.emplace_back(CD3DX12_VIEWPORT(0.0f, 0.0f, (float)draw_data->DisplaySize.x, (float)draw_data->DisplaySize.y));
		context->SetViewports(viewports);

		context->SetPipeline(bd->pipeline_imgui_);
		context->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto test = std::make_pair<std::shared_ptr<madX::Engine::Buffer>, std::size_t>(std::move(vb), (std::size_t)0);
		std::vector<madX::Engine::VertexStream> test2 = { test };
		context->SetVertexBuffer(0, test2);
		context->SetIndexBuffer(ib, 0u);

		context->CommitShaderResources();

		auto root_signature = bd->pipeline_imgui_->GetRootSignature();
		command_list->SetGraphicsRootSignature(root_signature);

		context->CommitConstantBuffers(vs_constants);

		////////////////////////////////////////////////////////////////////////////////

		int vtx_offset = 0;
		int idx_offset = 0;

		ImVec2 clip = draw_data->DisplayPos;

		auto fonts = ImGui::GetIO().Fonts->TexID;

		for (int list_index = 0; list_index < draw_data->CmdListsCount; ++list_index)
		{
			const auto cmd_list = draw_data->CmdLists[list_index];

			for (int cmd_index = 0; cmd_index < cmd_list->CmdBuffer.Size; ++cmd_index)
			{
				const auto cmd = &cmd_list->CmdBuffer[cmd_index];

				if (cmd->UserCallback != nullptr)
				{
					//TODO: something useful...
					assert(false);
				}
				else
				{
					const ImVec2 clip_min(cmd->ClipRect.x - clip.x, cmd->ClipRect.y - clip.y);
					const ImVec2 clip_max(cmd->ClipRect.z - clip.x, cmd->ClipRect.w - clip.y);

					if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
					{
						continue;
					}

					if (auto texture = cmd->GetTexID(); texture)
					{			
						D3D12_GPU_DESCRIPTOR_HANDLE texture_handle = { (UINT64)texture };
						command_list->SetGraphicsRootDescriptorTable(1, texture_handle);

						//TODO: context!
						if (fonts == texture)
						{
							command_list->SetPipelineState(bd->pipeline_imgui_->GetPipelineState());
						}
						else
						{
							command_list->SetPipelineState(bd->pipeline_raw_->GetPipelineState());
						}
					}

					const D3D12_RECT scisors = { (LONG)clip_min.x, (LONG)clip_min.y, (LONG)clip_max.x, (LONG)clip_max.y };
					context->SetScissorRects(1, &scisors);

					madX::Engine::DrawIndexedAttributes draw;
					draw.indices_ = cmd->ElemCount;
					draw.first_index_index_ = cmd->IdxOffset + idx_offset;
					draw.base_vertex_ = cmd->VtxOffset + vtx_offset;
					context->DrawIndexed(draw);
				}
			}

			vtx_offset += cmd_list->VtxBuffer.Size;
			idx_offset += cmd_list->IdxBuffer.Size;
		}
	}

	void PlatformIO_CreateWindow(ImGuiViewport* viewport)
	{
		BackendData* bd = GetBackendData();
		ViewportData* vd = IM_NEW(ViewportData)();
		viewport->RendererUserData = vd;

		assert(viewport->PlatformHandleRaw);
		//HWND hwnd = viewport->PlatformHandleRaw ? (HWND)viewport->PlatformHandleRaw : (HWND)viewport->PlatformHandle;
		HWND hwnd = (HWND)viewport->PlatformHandleRaw;

		////////////////////////////////////////////////////////////////////////////////

		auto renderer = bd->renderer_.get();

		vd->commands_ = std::make_unique<madX::Engine::Commands>
		(
			renderer,
			madX::Engine::cEngine_BackBufferCount,
		    madX::Engine::cEngine_CommandListPerFrame,
			renderer->GetGraphicsQueue()->GetDesc()
		);

		vd->context_ = std::make_unique<madX::Engine::Context>(renderer);
		//auto command_list = vd->commands_->GetNewCommandList();
		//vd->context_->SetCommandList(command_list);
		
		madX::Engine::SwapChainDesc swapchain_desc;
		swapchain_desc.name_ = "SwapChain::Viewport" + std::to_string(viewport->ID);
		swapchain_desc.width_ = (UINT)viewport->Size.x;
		swapchain_desc.height_ = (UINT)viewport->Size.y;
		//swapchain_desc.format_ = 
		swapchain_desc.window_ = hwnd;
		vd->swapchain_ = bd->renderer_->CreateSwapChain(swapchain_desc);

		return;
	};

	void PlatformIO_DestroyWindow(ImGuiViewport* viewport)
	{
		if (ViewportData* vd = (ViewportData*)viewport->RendererUserData)
		{
			assert(vd != NULL);
			IM_DELETE(vd);
		}

		viewport->RendererUserData = NULL;
	}

	void PlatformIO_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
	{
		BackendData* bd = GetBackendData();
		ViewportData* vd = (ViewportData*)viewport->RendererUserData;

		//TODO: separate command queue
		bd->renderer_->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		bd->renderer_->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);
		vd->swapchain_->Resize((UINT)size.x, (UINT)size.y);
	}

	void PlatformIO_RenderWindow(ImGuiViewport* viewport, void*)
	{
		BackendData* bd = GetBackendData();
		ViewportData* vd = (ViewportData*)viewport->RendererUserData;

		auto commands = vd->commands_.get();
		commands->BeginFrame(vd->frame_);

		auto command_list = commands->GetNewCommandList();
		vd->context_->SetCommandList(command_list);

		auto renderer = bd->renderer_.get();
		auto swapchain = vd->swapchain_.get();

		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				swapchain->GetBackBufferResource(),
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			);

			command_list->ResourceBarrier(1, &bb_barier);
		}

		////////////////////////////////////////////////////////////////////////////////
		
		{
			auto result = command_list->Close();
			madX::Engine::ThrowIfFailed(result, "[PlatformIO_RenderWindow at swapchain::wait] ID3D12GraphicsCommandList::Close failed");

			ID3D12CommandList* const lists[] = { command_list };
			renderer->GetGraphicsQueue()->ExecuteCommandLists(1, lists);
		}

		swapchain->Wait(vd->frame_);

		////////////////////////////////////////////////////////////////////////////////

		command_list = commands->GetNewCommandList();
		vd->context_->SetCommandList(command_list);

		command_list->OMSetRenderTargets(1, swapchain->GetBackBufferHandle(), TRUE, NULL);

		float clear_color[] = { 0.3f, 0.3f, 0.3f, 1.0f }; //TODO: not seen
		command_list->ClearRenderTargetView(*swapchain->GetBackBufferHandle(), clear_color, 0, NULL);

		RenderData(viewport->DrawData, command_list);
	}

	void PlatformIO_SwapBuffers(ImGuiViewport* viewport, void*)
	{
		BackendData* bd = GetBackendData();
		ViewportData* vd = (ViewportData*)viewport->RendererUserData;

		auto renderer = bd->renderer_.get();
		auto swapchain = vd->swapchain_.get();

		auto command_list = vd->context_->GetCommandList();

		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				swapchain->GetBackBufferResource(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT
			);

			command_list->ResourceBarrier(1, &bb_barier);
		}
		
		{
			auto result = command_list->Close();
			madX::Engine::ThrowIfFailed(result, "[PlatformIO_RenderWindow at swapchain::present] ID3D12GraphicsCommandList::Close failed");
		
			ID3D12CommandList* const lists[] = { command_list };
			renderer->GetGraphicsQueue()->ExecuteCommandLists(1, lists);
		}
		
		////////////////////////////////////////////////////////////////////////////////
		
		swapchain->Present(vd->frame_++);
	}

	////////////////////////////////////////////////////////////////////////////////

	Backend::Backend(HWND window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigDockingWithShift = true;
		io.MouseDrawCursor = true;
		io.ConfigWindowsMoveFromTitleBarOnly = true;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;

		//	if (ImGuiStyle& style = ImGui::GetStyle(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		//	{
		//		style.WindowRounding = 0.0f;
		//		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		//	}

		ImGui_ImplWin32_Init(window);

		RECT rect;
		::SetRectEmpty(&rect);
		::GetClientRect(window, &rect);
		io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

		////////////////////////////////////////////////////////////////////////////////

		auto renderer = std::make_shared<madX::Engine::Renderer>();

		madX::Engine::SwapChainDesc swapchain_desc;
		swapchain_desc.name_ = "MainSwapChain";
		swapchain_desc.window_ = window;
		auto swapchain = renderer->CreateSwapChain(swapchain_desc);

		panels_.emplace(std::make_pair("Debug", std::make_shared<Panel::Debug>(renderer)));
		panels_.emplace(std::make_pair("Properties", std::make_shared<Panel::Properties>()));
		panels_.emplace(std::make_pair("NodesEditor", std::make_shared<Panel::NodesEditor>()));
			
		////////////////////////////////////////////////////////////////////////////////

		//ImGuiIO& io = ImGui::GetIO();
		assert(io.BackendRendererUserData == NULL);

		BackendData* bd = IM_NEW(BackendData)();
		io.BackendRendererUserData = (void*)bd;
		io.BackendRendererName = "madX::Backend";
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{		
			ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
			platform_io.Renderer_CreateWindow = PlatformIO_CreateWindow;
			platform_io.Renderer_DestroyWindow = PlatformIO_DestroyWindow;
			platform_io.Renderer_SetWindowSize = PlatformIO_SetWindowSize;
			platform_io.Renderer_RenderWindow = PlatformIO_RenderWindow;
			platform_io.Renderer_SwapBuffers = PlatformIO_SwapBuffers;
		}
		
		bd->renderer_ = std::move(renderer);
		bd->swapchain_ = std::move(swapchain);
		bd->context_ = std::make_unique<madX::Engine::Context>(renderer.get());
	
		ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		main_viewport->RendererUserData = IM_NEW(ViewportData)();

		//::ShowCursor(FALSE);
	}

	Backend::~Backend()
	{
		BackendData* bd = GetBackendData();
		assert(bd != NULL);

		//project_->Destroy();

		ImGuiIO& io = ImGui::GetIO();

		//	for (auto& pass : pass_)
		//	{
		//		pass->Destroy();
		//	}

		ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		if (ViewportData* vd = (ViewportData*)main_viewport->RendererUserData)
		{
			//	for (UINT i = 0; i < bd->numFramesInFlight; i++)
			//	{
			//		ImGui_ImplDX12_DestroyRenderBuffers(&vd->FrameRenderBuffers[i]);
			//	}

			IM_DELETE(vd);
			main_viewport->RendererUserData = NULL;
		}

		//ImGui_ImplDX12_ShutdownPlatformInterface();
		ImGui::DestroyPlatformWindows();
		
		//TODO: implement it
		//ImGui_ImplDX12_InvalidateDeviceObjects();

		io.BackendRendererName = NULL;
		io.BackendRendererUserData = NULL;

		IM_DELETE(bd);

		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		////////////////////////////////////////////////////////////////////////////////

	//	for (const auto& panel : panels_)
	//	{
	//		const auto& state = panel.second->GetPanelState();
	//	
	//		auto test = cista::serialize(state);
	//	
	//	
	//		continue;
	//	}
	}

	void Backend::Create()
	{
		BackendData* bd = GetBackendData();
		assert(bd != NULL);

		auto renderer = bd->renderer_.get();

		project_ = std::make_shared<Root::Component::Project>(bd->renderer_);
		
//		//TODO: meta
//		for (auto& panel : panels_)
//		{
//			panel.second->CreatePanel(processor_);	
//		}

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::PipelineDesc pipeline_desc;

		pipeline_desc.RT_count_ = 1;
		pipeline_desc.RTV_format[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		pipeline_desc.DSV_format = DXGI_FORMAT_UNKNOWN;
		pipeline_desc.depth_stencil_state_.DepthEnable = FALSE;
		pipeline_desc.rasterizer_state_.CullMode = D3D12_CULL_MODE_NONE;

		{
			madX::Engine::ShaderDesc shader_desc;

			shader_desc.name_ = "imguiVS";
			shader_desc.type_ = madX::Engine::ShaderType::Vertex;
			shader_desc.file_ = "shaders\\imgui.vsh";
			shader_desc.entry_ = L"main";
			//TODO: template for automatic creation based on <shader_desc>
			pipeline_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);

			shader_desc.name_ = "imguiPS";
			shader_desc.type_ = madX::Engine::ShaderType::Pixel;
			shader_desc.file_ = "shaders\\imgui.psh";
			shader_desc.entry_ = L"main";
			//TODO: template for automatic creation based on <shader_desc>
			pipeline_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);
		}

		pipeline_desc.primitive_topology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		pipeline_desc.input_layout_.push_back(
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

		pipeline_desc.input_layout_.push_back(
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

		pipeline_desc.input_layout_.push_back(
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

		////////////////////////////////////////////////////////////////////////////////

		pipeline_desc.name_ = "EditorRaw";
		bd->pipeline_raw_ = renderer->CreatePipeline(pipeline_desc);

		pipeline_desc.name_ = "EditorImGui";
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
		
		bd->pipeline_imgui_ = renderer->CreatePipeline(pipeline_desc);

		////////////////////////////////////////////////////////////////////////////////
		
		ImGuiIO& io = ImGui::GetIO();

		{
			unsigned char* fonts_pixels = nullptr;
			int fonts_width = 0;
			int fonts_height = 0;

			//ImFontConfig config;
			//config.OversampleH = 3;
			//config.OversampleV = 1;
			//config.GlyphExtraSpacing.x = 0.5f;
			//ImFont* font = io.Fonts->AddFontFromFileTTF("fonts\\ProggyTiny.ttf", 13.0f, &config, io.Fonts->GetGlyphRangesDefault());
			io.Fonts->GetTexDataAsRGBA32(&fonts_pixels, &fonts_width, &fonts_height);

			madX::Engine::TextureDesc texture_desc;
			texture_desc.name_ = "Texture";
			texture_desc.width_ = fonts_width;
			texture_desc.height_ = fonts_height;
			texture_desc.dimension_ = madX::Engine::ResourceDimension::Texture2D;
			texture_desc.format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
			texture_desc.usage_ = madX::Engine::ResourceUsage::Static;
			texture_desc.bind_ = madX::Engine::ResourceBind::ShaderResource;

			madX::Engine::TextureResourceData fonts_mip[] = { { fonts_pixels, 0, 0 } };
			madX::Engine::TextureData fonts_data(fonts_mip, _countof(fonts_mip));

			bd->fonts_ = renderer->CreateTexture(texture_desc, &fonts_data);

			madX::Engine::TextureViewDesc view_desc;
			view_desc.type_ = madX::Engine::ResourceView::ShaderResource;

			bd->fonts_->CreateView(renderer, view_desc, bd->fonts_srv_);

			//////io.Fonts->TexID = (ImTextureID)bd->pipeline_imgui_->Bind(renderer, bd->fonts_srv_).ptr;	
			io.Fonts->TexID = (ImTextureID)bd->fonts_srv_.GetHandleGPU().ptr;
		}

		////////////////////////////////////////////////////////////////////////////////
		
		//TODO: make it happen in Renderer, in one shot
		{
			auto& upload = renderer->GetHeapUpload();
		
			renderer->UploadStaticData(upload.GetCommandList());
			upload.Flush(renderer);
			renderer->FreeUploadStaticHeap();
		}

		{	
			std::uint64_t width = (std::uint64_t)io.DisplaySize.x;
			std::uint64_t height = (std::uint64_t)io.DisplaySize.y;

			std::uint64_t size = (width & 0xffff) + (height << 16u);
			size_.store(std::uint64_t(size));
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	void Backend::MessagePump()
	{
		static bool s_in_sizemove = false;
		static bool s_in_suspend = false;
		static bool s_minimized = false;
		static bool s_fullscreen = false;

		//std::lock_guard<std::mutex> guard(lock_);

		while (false == queue_.empty())
		{
			auto message = queue_.front();
			queue_.pop();

			switch (message.id_)
			{
		//	case WM_CLOSE:
		//	{
		//		worker_.request_stop();
		//		return;
		//	}

		//	case WM_MOUSEMOVE:
		//	{
		//		//if (cursor_clip_)
		//		{
		//			POINT mouse;
		//			mouse.x = LOWORD(message.l_param_);
		//			mouse.y = HIWORD(message.l_param_);
		//		
		//			POINT teleport = mouse;
		//		
		//			RECT clip;
		//			//::SetRectEmpty(&clip);
		//			::GetClipCursor(&clip);
		//	
		//			::MapWindowPoints(HWND_DESKTOP, message.hwnd_, (LPPOINT)(&clip), 2);
		//		
		//			const int posMargin = 3;
		//		
		//			if (mouse.x < clip.left + posMargin)
		//			{
		//				teleport.x = clip.right - (2 * posMargin);
		//			}
		//			else if (mouse.x > clip.right - posMargin)
		//			{
		//				teleport.x = clip.left + (2 * posMargin);
		//			}
		//		
		//			if (mouse.y < clip.top + posMargin)
		//			{
		//				teleport.y = clip.bottom - (2 * posMargin);
		//			}
		//			else if (mouse.y > clip.bottom - posMargin)
		//			{
		//				teleport.y = clip.top + (2 * posMargin);
		//			}
		//		
		//			if (mouse.x != teleport.x || mouse.y != teleport.y)
		//			{
		//			//	POINT point;
		//			//	point.x = teleport.x;
		//			//	point.y = teleport.y;
		//		
		//				//TODO: handle returns
		//				::MapWindowPoints(message.hwnd_, HWND_DESKTOP, &teleport, 1);
		//				::SetCursorPos(teleport.x, teleport.y);
		//		
		//				ImGuiIO& io = ImGui::GetIO();
		//				io.MousePosPrev = ImVec2(-FLT_MAX, -FLT_MAX);
		//				io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
		//				io.MouseDelta = ImVec2();
		//	
		//			//	break;
		//			}
		//		}
		//	
		//		//context->IO.MousePos = ImVec2((float)mouse.x, (float)mouse.y);
		//		break;
		//	}

			case WM_SIZE:
			{
				if (message.w_param_ == SIZE_MINIMIZED)
				{
					if (!s_minimized)
					{
						if (s_minimized = true; !s_in_suspend)
						{
							OnSuspending();
						}
						s_in_suspend = true;
					}
				}
				else if (s_minimized)
				{
					if (s_minimized = false; s_in_suspend)
					{
						OnResuming();
					}
					s_in_suspend = false;
				}
				//else if (!s_in_sizemove && backend)
				//else if (s_in_sizemove && backend)
				else
				{
					size_.store(std::uint64_t(message.l_param_));
				}

				break;
			}

			case WM_ENTERSIZEMOVE:
			{
				s_in_sizemove = true;
				break;
			}

			case WM_EXITSIZEMOVE:
			{
				s_in_sizemove = false;

				RECT rc;
				GetClientRect(message.hwnd_, &rc);

				std::uint64_t size = (std::uint64_t(rc.bottom - rc.top) << 16u) + (std::uint64_t(rc.right - rc.left) & 0xffff);
				size_.store(std::uint64_t(size));
				break;
			}

			case WM_ACTIVATEAPP:
			{
				if (message.w_param_)
				{
					OnActivated();
				}
				else
				{
					OnDeactivated();
				}
				break;
			}

			//	case WM_SYSKEYDOWN:
			//	{
			//		if (w_param == VK_RETURN && (l_param & 0x60000000) == 0x20000000)
			//		{
			//			// Implements the classic ALT+ENTER fullscreen toggle
			//			if (s_fullscreen)
			//			{
			//				SetWindowLongPtr(handle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			//				SetWindowLongPtr(handle, GWL_EXSTYLE, 0);
			//
			//				int width = 800;
			//				int height = 600;
			//				if (backend)
			//					backend->GetDefaultSize(width, height);
			//
			//				ShowWindow(handle, SW_SHOWNORMAL);
			//
			//				SetWindowPos(handle, handle_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
			//			}
			//			else
			//			{
			//				SetWindowLongPtr(handle, GWL_STYLE, WS_POPUP);
			//				SetWindowLongPtr(handle, GWL_EXSTYLE, WS_EX_TOPMOST);
			//
			//				SetWindowPos(handle, handle_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
			//
			//				ShowWindow(handle, SW_SHOWMAXIMIZED);
			//			}
			//
			//			s_fullscreen = !s_fullscreen;
			//		}
			//		break;
			//	}
			default:
			{
				break;
			}
			}
			
			ImGui_ImplWin32_WndProcHandler(message.hwnd_, message.id_, message.w_param_, message.l_param_);
			}
	}

	void Backend::Update()
	{
		MessagePump();

		BackendData* bd = GetBackendData();
		assert(bd != NULL);

		auto renderer = bd->renderer_.get();
		auto swapchain = bd->swapchain_.get();

		auto size = size_.load();
		{
			bd->renderer_->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
			bd->renderer_->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);
			bd->swapchain_->Resize(std::uint32_t(size & 0xffff), std::uint32_t(size >> 16u));
		}

		for (auto& panel : panels_)
		{
			panel.second->UpdatePanel();
		}
	}

	void Backend::Begin()
	{
		BackendData* bd = GetBackendData();
		assert(bd != NULL);

		auto renderer = bd->renderer_.get();
		auto swapchain = bd->swapchain_.get();
		renderer->BeginFrame();

		auto command_list = renderer->GetNewCommandList();

		auto barier = CD3DX12_RESOURCE_BARRIER::Transition(swapchain->GetBackBufferResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		command_list->ResourceBarrier(1, &barier);

		////////////////////////////////////////////////////////////////////////////////

		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		for (auto& panel : panels_)
		{
			panel.second->BeginPanel();
		}
	}

	void Backend::Process()
	{
		BackendData* bd = GetBackendData();
		assert(bd != NULL);

		auto renderer = bd->renderer_.get();
		//auto swapchain = bd->swapchain_.get();

		auto command_list = renderer->GetCurrentCommandList();

		////////////////////////////////////////////////////////////////////////////////

		{
			ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			{
				window_flags |= ImGuiWindowFlags_NoBackground;
			}

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("madX::DockSpaceWindow", NULL, window_flags);
			ImGui::PopStyleVar();

			ImGui::PopStyleVar(2);

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("madX::DockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}

		//	if (ImGui::BeginMenuBar())
		//	{
		//		if (ImGui::BeginMenu("Options"))
		//		{
		//			ImGui::EndMenu();
		//		}
		//	
		//		ImGui::EndMenuBar();
		//	}
			
			ImGui::End();
		}

		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////

		ImGui::ShowDemoWindow();

		auto& debug = panels_.at("Debug");
		auto& nodes = panels_.at("NodesEditor");


		//GOAT-begin
		debug->ProcessPanel(processor_);
		nodes->ProcessPanel(processor_);
		//const auto& nodes_state = (const Panel::NodesEditorState&)nodes->GetPanelState();

		processor_.ProcessNodes(nodes->GetPanelState());

		auto properties = static_cast<Panel::Properties*>(panels_.at("Properties").get());
		properties->ProcessPanel(processor_);
		//GOAT-end

	//	for (auto& panel : panels_)
	//	{
	//		panel.second->ProcessPanel(project_);
	//	}
	}

	void Backend::End()
	{
		for (auto& panel : panels_)
		{
			panel.second->EndPanel();
		}

		////////////////////////////////////////////////////////////////////////////////

		BackendData* bd = GetBackendData();
		assert(bd != NULL);

		auto renderer = bd->renderer_.get();
		auto swapchain = bd->swapchain_.get();

		auto command_list = renderer->GetCurrentCommandList();

		{
			auto result = command_list->Close();
			madX::Engine::ThrowIfFailed(result, "[Engine::Process(Event::End) at swapchain::wait] ID3D12GraphicsCommandList::Close failed");

			ID3D12CommandList* const lists[] = { command_list };
			renderer->GetGraphicsQueue()->ExecuteCommandLists(1, lists);
		}

		swapchain->Wait(renderer->GetFrame());

		////////////////////////////////////////////////////////////////////////////////

		command_list = renderer->GetNewCommandList();

		command_list->OMSetRenderTargets(1, swapchain->GetBackBufferHandle(), TRUE, NULL);
		float clear_color[] = { 0.3f, 0.3f, 0.3f, 1.0f }; //TODO: not seen
		command_list->ClearRenderTargetView(*swapchain->GetBackBufferHandle(), clear_color, 0, NULL);
		//command_list->ClearRenderTargetView(*swapchain->GetBackBufferHandle(), madX::Engine::cTextureClearValue_ColorZero.data(), 0, NULL);

		ImGui::Render();
		RenderData(ImGui::GetDrawData(), command_list);

		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition(swapchain->GetBackBufferResource(), D3D12_RESOURCE_STATE_RENDER_TARGET,
			  D3D12_RESOURCE_STATE_PRESENT);
			command_list->ResourceBarrier(1, &bb_barier);
		}

		{
			auto result = command_list->Close();
			madX::Engine::ThrowIfFailed(result, "[Engine::Process(Event::End) at swapchain::present] ID3D12GraphicsCommandList::Close failed");

			ID3D12CommandList* const lists[] = { command_list };
			renderer->GetGraphicsQueue()->ExecuteCommandLists(1, lists);
		}

		if (ImGuiIO& io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(NULL, (void*)command_list);
		}

		////////////////////////////////////////////////////////////////////////////////

		swapchain->Present(renderer->GetFrame());

		//TODO: flush every frame for GPU Validation
		renderer->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		renderer->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);

		renderer->EndFrame();
	}

	void Backend::OnWindowSizeChanged(std::uint32_t width, std::uint32_t height)
	{
		BackendData* bd = GetBackendData();
		assert(bd != NULL);
		
		assert(bd->renderer_);
		assert(bd->swapchain_);
		
		//TODO: separate command queue
		bd->renderer_->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		bd->renderer_->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);
		bd->swapchain_->Resize(width, height);
	}
}