#include "editor.hpp"

//#include <cassert>

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
		//io.MouseDrawCursor = true;
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		io.ConfigWindowsResizeFromEdges = true;
		io.ConfigViewportsNoTaskBarIcon = true;

		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;

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

		//::ShowCursor(FALSE);
	}

	EditorUI::~EditorUI()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void EditorUI::Create()
	{
		//		////////////////////////////////////////////////////////////////////////////////
		//		////////////////////////////////////////////////////////////////////////////////
		//		////////////////////////////////////////////////////////////////////////////////
		//	
		//		Framegraph::Frame frame;
		//		auto pass = frame.AddPass<void>
		//			(
		//				"test",
		//				Framegraph::PassType::Undefined,
		//				Framegraph::PassFlags::None,
		//				nullptr,
		//				nullptr
		//			);
		//	
		//	
		//		////////////////////////////////////////////////////////////////////////////////
		//		////////////////////////////////////////////////////////////////////////////////
		//		////////////////////////////////////////////////////////////////////////////////
	}

	void EditorUI::Update()
	{
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
	}

	void EditorUI::Process()
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
		
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{
				ImGui::EndMenu();
			}
		
			ImGui::EndMenuBar();
		}
		
		ImGui::End();
		
		////////////////////////////////////////////////////////////////////////////////
		
		ImGui::ShowDemoWindow();
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
	}

	void EditorUI::Destroy()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
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