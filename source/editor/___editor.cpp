#include "editor.hpp"
#include "ui_imgui.hpp"

#include <cassert>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace madX::Editor
{
	EditorUI::EditorUI()
	{
		std::shared_ptr<madX::Platform::Window> window = madX::Platform::Window::GetInstance();
		window->RegisterMessagePump(ImGui_ImplWin32_WndProcHandler);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigDockingWithShift = true;
		io.MouseDrawCursor = true;
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		//io.ConfigWindowsResizeFromEdges = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		//ImGuiStyle& style = ImGui::GetStyle();
		//style.Colors[ImGuiCol_WindowBg].w = 1.0f;

		//	if (ImGuiStyle& style = ImGui::GetStyle(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		//	{
		//		style.WindowRounding = 0.0f;
		//		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		//	}

		////////////////////////////////////////////////////////////////////////////////

		//	RECT rect = window->GetClientRect();
		//	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

		processor_ = std::make_shared<madX::Backend::Processor>();
		auto device = processor_->GetDevice();

		madX::Backend::SwapChainDesc swapchain_desc;
		swapchain_desc.name_ = "MainSwapChain";
		swapchain_desc.window_ = window->GetHandle();
		swapchain_ = processor_->CreateSwapChain(swapchain_desc);

		element_ = processor_->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 0x20);

		ImGui_ImplWin32_Init(window->GetHandle());
		auto result = ImGui_ImplDX12_Init
		(
			device,
			Backend::cEngine_BackBufferCount,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			element_.GetDescriptorHeap(),
			element_.GetHandleCPU(0),
			element_.GetHandleGPU(0)
		);

		////////////////////////////////////////////////////////////////////////////////

		////////////		//ImGuiIO& io = ImGui::GetIO();
		////////////		assert(io.BackendRendererUserData == NULL);
		////////////		
		////////////		ImGuiBackendData* ibd = IM_NEW(ImGuiBackendData)();
		////////////		io.BackendRendererUserData = (void*)ibd;
		////////////		io.BackendRendererName = "madX::Backend";
		////////////		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
		////////////		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
		////////////		
		////////////		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		////////////		{
		////////////			ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
		////////////			platform_io.Renderer_CreateWindow = ImGuiCreateWindow;
		////////////			platform_io.Renderer_DestroyWindow = ImGuiDestroyWindow;
		////////////			platform_io.Renderer_SetWindowSize = ImGuiSetWindowSize;
		////////////			platform_io.Renderer_RenderWindow = ImGuiRenderWindow;
		////////////			platform_io.Renderer_SwapBuffers = ImGuiSwapBuffers;
		////////////		}
		////////////		
		////////////		ibd->processor_ = std::move(processor);
		////////////		ibd->swapchain_ = std::move(swapchain);
		////////////		ibd->context_ = std::make_unique<madX::Backend::Context>(processor.get());
		////////////		
		////////////		ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		////////////		main_viewport->RendererUserData = IM_NEW(ImGuiBackendViewportData)();

		//::ShowCursor(FALSE);
	}

	EditorUI::~EditorUI()
	{
		////////////		ImGuiBackendData* ibd = GetImGuiBackendData();
		////////////		assert(ibd != NULL);
		////////////		
		////////////		//project_->Destroy();
		////////////		
		////////////		ImGuiIO& io = ImGui::GetIO();
		////////////		
		////////////		//	for (auto& pass : pass_)
		////////////		//	{
		////////////		//		pass->Destroy();
		////////////		//	}
		////////////		
		////////////		ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		////////////		if (ImGuiBackendViewportData* vd = (ImGuiBackendViewportData*)main_viewport->RendererUserData)
		////////////		{
		////////////			//	for (UINT i = 0; i < fd->numFramesInFlight; i++)
		////////////			//	{
		////////////			//		ImGui_ImplDX12_DestroyRenderBuffers(&vd->FrameRenderBuffers[i]);
		////////////			//	}
		////////////		
		////////////			IM_DELETE(vd);
		////////////			main_viewport->RendererUserData = NULL;
		////////////		}
		////////////		
		////////////		//ImGui_ImplDX12_ShutdownPlatformInterface();
		////////////		ImGui::DestroyPlatformWindows();
		////////////		
		////////////		//TODO: implement it
		////////////		//ImGui_ImplDX12_InvalidateDeviceObjects();
		////////////		
		////////////		io.BackendRendererName = NULL;
		////////////		io.BackendRendererUserData = NULL;
		////////////		
		////////////		IM_DELETE(ibd);
		////////////		
		////////////		ImGui_ImplWin32_Shutdown();
		////////////		ImGui::DestroyContext();
		////////////		
		////////////		////////////////////////////////////////////////////////////////////////////////
		////////////		
		////////////		//	for (const auto& panel : panels_)
		////////////		//	{
		////////////		//		const auto& state = panel.second->GetPanelState();
		////////////		//	
		////////////		//		auto test = cista::serialize(state);
		////////////		//	
		////////////		//	
		////////////		//		continue;
		////////////		//	}
	}

	void EditorUI::Create()
	{
		////////////		//		////////////////////////////////////////////////////////////////////////////////
		////////////		//		////////////////////////////////////////////////////////////////////////////////
		////////////		//		////////////////////////////////////////////////////////////////////////////////
		////////////		//	
		////////////		//		Framegraph::Frame frame;
		////////////		//		auto pass = frame.AddPass<void>
		////////////		//			(
		////////////		//				"test",
		////////////		//				Framegraph::PassType::Undefined,
		////////////		//				Framegraph::PassFlags::None,
		////////////		//				nullptr,
		////////////		//				nullptr
		////////////		//			);
		////////////		//	
		////////////		//	
		////////////		//		////////////////////////////////////////////////////////////////////////////////
		////////////		//		////////////////////////////////////////////////////////////////////////////////
		////////////		//		////////////////////////////////////////////////////////////////////////////////
		////////////		
		////////////		ImGuiBackendData* ibd = GetImGuiBackendData();
		////////////		assert(ibd != NULL);
		////////////		
		////////////		auto renderer = ibd->processor_.get();
		////////////		
		////////////		////	project_ = std::make_shared<Root::Component::Project>(fd->processor_);
		////////////		
		////////////		//		//TODO: meta
		////////////		//		for (auto& panel : panels_)
		////////////		//		{
		////////////		//			panel.second->CreatePanel(processor_);	
		////////////		//		}
		////////////		
		////////////		////////////////////////////////////////////////////////////////////////////////
		////////////		
		////////////		madX::Backend::PipelineDesc pipe_desc;
		////////////		
		////////////		pipe_desc.RT_count_ = 1;
		////////////		pipe_desc.RTV_format[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		////////////		
		////////////		pipe_desc.DSV_format = DXGI_FORMAT_UNKNOWN;
		////////////		pipe_desc.depth_stencil_state_.DepthEnable = FALSE;
		////////////		pipe_desc.rasterizer_state_.CullMode = D3D12_CULL_MODE_NONE;
		////////////		
		////////////		{
		////////////			madX::Backend::ShaderDesc shader_desc;
		////////////		
		////////////			shader_desc.name_ = "imguiVS";
		////////////			shader_desc.type_ = madX::Backend::ShaderType::Vertex;
		////////////			shader_desc.file_ = "shaders\\imgui.vsh";
		////////////			shader_desc.entry_ = L"main";
		////////////			//TODO: template for automatic creation based on <shader_desc>
		////////////			pipe_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);
		////////////		
		////////////			shader_desc.name_ = "imguiPS";
		////////////			shader_desc.type_ = madX::Backend::ShaderType::Pixel;
		////////////			shader_desc.file_ = "shaders\\imgui.psh";
		////////////			shader_desc.entry_ = L"main";
		////////////			//TODO: template for automatic creation based on <shader_desc>
		////////////			pipe_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);
		////////////		}
		////////////		
		////////////		pipe_desc.primitive_topology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		////////////		
		////////////		pipe_desc.input_layout_.push_back(
		////////////			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		////////////		
		////////////		pipe_desc.input_layout_.push_back(
		////////////			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		////////////		
		////////////		pipe_desc.input_layout_.push_back(
		////////////			{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		////////////		
		////////////		////////////////////////////////////////////////////////////////////////////////
		////////////		
		////////////		pipe_desc.name_ = "EditorRaw";
		////////////		ibd->pipe_raw_ = renderer->CreatePipeline(pipe_desc);
		////////////		
		////////////		pipe_desc.name_ = "EditorImGui";
		////////////		auto& blend_state = pipe_desc.blend_state_.RenderTarget[0];
		////////////		
		////////////		blend_state.BlendEnable = TRUE;
		////////////		blend_state.LogicOpEnable = FALSE;
		////////////		blend_state.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		////////////		blend_state.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		////////////		blend_state.BlendOp = D3D12_BLEND_OP_ADD;
		////////////		blend_state.SrcBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		////////////		blend_state.DestBlendAlpha = D3D12_BLEND_ZERO;
		////////////		blend_state.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		////////////		blend_state.LogicOp = D3D12_LOGIC_OP_NOOP;
		////////////		blend_state.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		////////////		
		////////////		ibd->pipe_imgui_ = renderer->CreatePipeline(pipe_desc);
		////////////		
		////////////		////////////////////////////////////////////////////////////////////////////////
		////////////		
		////////////		ImGuiIO& io = ImGui::GetIO();
		////////////		
		////////////		{
		////////////			unsigned char* fonts_pixels = nullptr;
		////////////			int fonts_width = 0;
		////////////			int fonts_height = 0;
		////////////		
		////////////			//ImFontConfig config;
		////////////			//config.OversampleH = 3;
		////////////			//config.OversampleV = 1;
		////////////			//config.GlyphExtraSpacing.x = 0.5f;
		////////////			//ImFont* font = io.Fonts->AddFontFromFileTTF("fonts\\ProggyTiny.ttf", 13.0f, &config, io.Fonts->GetGlyphRangesDefault());
		////////////			io.Fonts->GetTexDataAsRGBA32(&fonts_pixels, &fonts_width, &fonts_height);
		////////////		
		////////////			madX::Backend::TextureDesc texture_desc;
		////////////			texture_desc.name_ = "Texture";
		////////////			texture_desc.width_ = fonts_width;
		////////////			texture_desc.height_ = fonts_height;
		////////////			texture_desc.dimension_ = madX::Backend::ResourceDimension::Texture2D;
		////////////			texture_desc.format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
		////////////			texture_desc.usage_ = madX::Backend::ResourceUsage::Static;
		////////////			texture_desc.bind_ = madX::Backend::ResourceBind::ShaderResource;
		////////////		
		////////////			madX::Backend::TextureResourceData fonts_mip[] = { { fonts_pixels, 0, 0 } };
		////////////			madX::Backend::TextureData fonts_data(fonts_mip, _countof(fonts_mip));
		////////////		
		////////////			ibd->fonts_ = renderer->CreateTexture(texture_desc, &fonts_data);
		////////////		
		////////////			madX::Backend::TextureViewDesc view_desc;
		////////////			view_desc.type_ = madX::Backend::ResourceView::ShaderResource;
		////////////		
		////////////			ibd->fonts_->CreateView(renderer, view_desc, ibd->fonts_srv_);
		////////////			io.Fonts->TexID = (ImTextureID)ibd->fonts_srv_.GetHandleGPU().ptr;
		////////////		}
		////////////		
		////////////		////////////////////////////////////////////////////////////////////////////////
		////////////		
		////////////		//TODO: make it happen in Renderer, in one shot
		////////////		{
		////////////			auto& upload = renderer->GetHeapUpload();
		////////////		
		////////////			renderer->UploadStaticData(upload.GetCommandList());
		////////////			upload.Flush(renderer);
		////////////			renderer->FreeUploadStaticHeap();
		////////////		}
		////////////		
		////////////		{
		////////////			std::uint64_t width = (std::uint64_t)io.DisplaySize.x;
		////////////			std::uint64_t height = (std::uint64_t)io.DisplaySize.y;
		////////////		
		////////////			std::uint64_t size = (width & 0xffff) + (height << 16u);
		////////////			size_.store(std::uint64_t(size));
		////////////		}
	}

	void EditorUI::Update()
	{
		////////////		ImGuiBackendData* ibd = GetImGuiBackendData();
		////////////		assert(ibd != NULL);
		////////////		
		////////////		auto renderer = ibd->processor_.get();
		////////////		auto swapchain = ibd->swapchain_.get();
		////////////		
		////////////		auto size = size_.load();
		////////////		{
		////////////			ibd->processor_->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		////////////			ibd->processor_->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);
		////////////			ibd->swapchain_->Resize(std::uint32_t(size & 0xffff), std::uint32_t(size >> 16u));
		////////////		}
		////////////		
		////////////		////	for (auto& panel : panels_)
		////////////		////	{
		////////////		////		panel.second->UpdatePanel();
		////////////		////	}
	}

	void EditorUI::Begin()
	{
		processor_->BeginFrame();
		
		auto command_list = processor_->GetNewCommandList();
		
		auto barier = CD3DX12_RESOURCE_BARRIER::Transition(swapchain_->GetBackBufferResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		command_list->ResourceBarrier(1, &barier);

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		////////////		ImGuiBackendData* ibd = GetImGuiBackendData();
		////////////		assert(ibd != NULL);
		////////////		
		////////////		auto renderer = ibd->processor_.get();
		////////////		auto swapchain = ibd->swapchain_.get();
		////////////		renderer->BeginFrame();
		////////////		
		////////////		auto command_list = renderer->GetNewCommandList();
		////////////		
		////////////		auto barier = CD3DX12_RESOURCE_BARRIER::Transition(swapchain->GetBackBufferResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		////////////		command_list->ResourceBarrier(1, &barier);
		////////////		
		////////////		////////////////////////////////////////////////////////////////////////////////
		////////////		
		////////////		ImGui_ImplWin32_NewFrame();
		////////////		ImGui::NewFrame();
		////////////		
		////////////		////	for (auto& panel : panels_)
		////////////		////	{
		////////////		////		panel.second->BeginPanel();
		////////////		////	}
	}

	void EditorUI::Process()
	{
		ImGui::ShowDemoWindow();

		////////////		ImGuiBackendData* ibd = GetImGuiBackendData();
		////////////		assert(ibd != NULL);
		////////////		
		////////////		auto renderer = ibd->processor_.get();
		////////////		//auto swapchain = ibd->swapchain_.get();
		////////////		
		////////////		auto command_list = renderer->GetCurrentCommandList();
		////////////		
		////////////		////////////////////////////////////////////////////////////////////////////////
		////////////		
		////////////		{
		////////////			ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		////////////			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		////////////		
		////////////			const ImGuiViewport* viewport = ImGui::GetMainViewport();
		////////////			ImGui::SetNextWindowPos(viewport->WorkPos);
		////////////			ImGui::SetNextWindowSize(viewport->WorkSize);
		////////////			ImGui::SetNextWindowViewport(viewport->ID);
		////////////			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		////////////			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		////////////			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		////////////			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		////////////		
		////////////			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		////////////			{
		////////////				window_flags |= ImGuiWindowFlags_NoBackground;
		////////////			}
		////////////		
		////////////			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		////////////			ImGui::Begin("madX::DockSpaceWindow", NULL, window_flags);
		////////////			ImGui::PopStyleVar();
		////////////		
		////////////			ImGui::PopStyleVar(2);
		////////////		
		////////////			ImGuiIO& io = ImGui::GetIO();
		////////////			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		////////////			{
		////////////				ImGuiID dockspace_id = ImGui::GetID("madX::DockSpace");
		////////////				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		////////////			}
		////////////		
		////////////			//	if (ImGui::BeginMenuBar())
		////////////			//	{
		////////////			//		if (ImGui::BeginMenu("Options"))
		////////////			//		{
		////////////			//			ImGui::EndMenu();
		////////////			//		}
		////////////			//	
		////////////			//		ImGui::EndMenuBar();
		////////////			//	}
		////////////		
		////////////			ImGui::End();
		////////////		}
		////////////		
		////////////		////////////////////////////////////////////////////////////////////////////////
		////////////		////////////////////////////////////////////////////////////////////////////////
		////////////		////////////////////////////////////////////////////////////////////////////////
		////////////		
		////////////		ImGui::ShowDemoWindow();
		////////////		
		////////////		////	auto& debug = panels_.at("Debug");
		////////////		////	auto& nodes = panels_.at("NodesEditor");
		////////////		////
		////////////		////	//GOAT-begin
		////////////		////	debug->ProcessPanel(processor_);
		////////////		////	nodes->ProcessPanel(processor_);
		////////////		////	//const auto& nodes_state = (const Panel::NodesEditorState&)nodes->GetPanelState();
		////////////		////	
		////////////		////	processor_.ProcessNodes(nodes->GetPanelState());
		////////////		////	
		////////////		////	auto properties = static_cast<Panel::Properties*>(panels_.at("Properties").get());
		////////////		////	properties->ProcessPanel(processor_);
		////////////		////	//GOAT-end
		////////////		
		////////////				//	for (auto& panel : panels_)
		////////////				//	{
		////////////				//		panel.second->ProcessPanel(project_);
		////////////				//	}
	}

	void EditorUI::End()
	{
		auto command_list = processor_->GetCurrentCommandList();
		
		{
			auto result = command_list->Close();
			madX::Backend::ThrowIfFailed(result, "[Engine::Process(Event::End) at swapchain::wait] ID3D12GraphicsCommandList::Close failed");
		
			ID3D12CommandList* const lists[] = { command_list };
			processor_->GetGraphicsQueue()->ExecuteCommandLists(1, lists);
		}
		
		swapchain_->Wait(processor_->GetFrame());
		
		////////////////////////////////////////////////////////////////////////////////
		
		command_list = processor_->GetNewCommandList();
		
		command_list->OMSetRenderTargets(1, swapchain_->GetBackBufferHandle(), TRUE, NULL);
		float clear_color[] = { 0.3f, 0.3f, 0.3f, 1.0f }; //TODO: not seen
		command_list->ClearRenderTargetView(*swapchain_->GetBackBufferHandle(), clear_color, 0, NULL);
		//command_list->ClearRenderTargetView(*swapchain->GetBackBufferHandle(), madX::Backend::cTextureClearValue_ColorZero.data(), 0, NULL);
		
		ImGui::Render();

		ID3D12DescriptorHeap* pp_heaps[] = { element_.GetDescriptorHeap() };
		command_list->SetDescriptorHeaps(_countof(pp_heaps), pp_heaps);

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command_list);
		
		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition(swapchain_->GetBackBufferResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			command_list->ResourceBarrier(1, &bb_barier);
		}
		
		{
			auto result = command_list->Close();
			madX::Backend::ThrowIfFailed(result, "[Engine::Process(Event::End) at swapchain::present] ID3D12GraphicsCommandList::Close failed");
		
			ID3D12CommandList* const lists[] = { command_list };
			processor_->GetGraphicsQueue()->ExecuteCommandLists(1, lists);
		}
		
		if (ImGuiIO& io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(NULL, (void*)command_list);
		}
		
		////////////////////////////////////////////////////////////////////////////////
		
		swapchain_->Present(processor_->GetFrame());
		
		//TODO: flush every frame for GPU Validation
		processor_->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		processor_->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);
		
		processor_->EndFrame();

		////////////		////	for (auto& panel : panels_)
		////////////		////	{
		////////////		////		panel.second->EndPanel();
		////////////		////	}
		////////////		
		////////////		////////////////////////////////////////////////////////////////////////////////
		////////////		
		////////////		ImGuiBackendData* ibd = GetImGuiBackendData();
		////////////		assert(ibd != NULL);
		////////////		
		////////////		auto renderer = ibd->processor_.get();
		////////////		auto swapchain = ibd->swapchain_.get();
		////////////		
		////////////		auto command_list = renderer->GetCurrentCommandList();
		////////////		
		////////////		{
		////////////			auto result = command_list->Close();
		////////////			madX::Backend::ThrowIfFailed(result, "[Engine::Process(Event::End) at swapchain::wait] ID3D12GraphicsCommandList::Close failed");
		////////////		
		////////////			ID3D12CommandList* const lists[] = { command_list };
		////////////			renderer->GetGraphicsQueue()->ExecuteCommandLists(1, lists);
		////////////		}
		////////////		
		////////////		swapchain->Wait(renderer->GetFrame());
		////////////		
		////////////		////////////////////////////////////////////////////////////////////////////////
		////////////		
		////////////		command_list = renderer->GetNewCommandList();
		////////////		
		////////////		command_list->OMSetRenderTargets(1, swapchain->GetBackBufferHandle(), TRUE, NULL);
		////////////		float clear_color[] = { 0.3f, 0.3f, 0.3f, 1.0f }; //TODO: not seen
		////////////		command_list->ClearRenderTargetView(*swapchain->GetBackBufferHandle(), clear_color, 0, NULL);
		////////////		//command_list->ClearRenderTargetView(*swapchain->GetBackBufferHandle(), madX::Backend::cTextureClearValue_ColorZero.data(), 0, NULL);
		////////////		
		////////////		ImGui::Render();
		////////////		ImGuiRenderData(ImGui::GetDrawData(), command_list);
		////////////		
		////////////		{
		////////////			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition(swapchain->GetBackBufferResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		////////////			command_list->ResourceBarrier(1, &bb_barier);
		////////////		}
		////////////		
		////////////		{
		////////////			auto result = command_list->Close();
		////////////			madX::Backend::ThrowIfFailed(result, "[Engine::Process(Event::End) at swapchain::present] ID3D12GraphicsCommandList::Close failed");
		////////////		
		////////////			ID3D12CommandList* const lists[] = { command_list };
		////////////			renderer->GetGraphicsQueue()->ExecuteCommandLists(1, lists);
		////////////		}
		////////////		
		////////////		if (ImGuiIO& io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		////////////		{
		////////////			ImGui::UpdatePlatformWindows();
		////////////			ImGui::RenderPlatformWindowsDefault(NULL, (void*)command_list);
		////////////		}
		////////////		
		////////////		////////////////////////////////////////////////////////////////////////////////
		////////////		
		////////////		swapchain->Present(renderer->GetFrame());
		////////////		
		////////////		//TODO: flush every frame for GPU Validation
		////////////		renderer->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		////////////		renderer->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);
		////////////		
		////////////		renderer->EndFrame();
	}

////	void Frontend::OnWindowSizeChanged(std::uint32_t width, std::uint32_t height)
////	{
////		ImGuiBackendData* ibd = GetImGuiBackendData();
////		assert(fd != NULL);
////	
////		assert(fd->processor_);
////		assert(fd->swapchain_);
////	
////		//TODO: separate command queue
////		fd->processor_->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
////		fd->processor_->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);
////		fd->swapchain_->Resize(width, height);
////	}
}