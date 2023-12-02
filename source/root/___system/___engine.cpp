#include "_root.hpp"

namespace madX::Root::System
{
	LRESULT Engine::Pump(HWND handle, UINT message, WPARAM w_param, LPARAM l_param)
	{
		switch (message)
		{
		case WM_KEYDOWN:
		{
			if (w_param == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}

			break;
		}
		case WM_SIZE:
		{
			RECT client = {};
			GetClientRect(handle, &client);

			auto registry = ECS::Registry::GetInstance();
			registry->Process(Root::Event::Resize { std::uint32_t(client.right - client.left), std::uint32_t(client.bottom - client.top) });
		}
		}

		return 0;
	}

	Engine::Engine()
	{
		renderer_ = std::make_shared<madX::Engine::Renderer>();

		auto window = madX::Core::Window::GetInstance();

		//TODO: event:create
		madX::Engine::SwapChainDesc swapchain_desc;
		swapchain_desc.name_ = "SwapChain";
		swapchain_desc.window_ = window;
		swapchain_ = renderer_->CreateSwapChain(swapchain_desc);

		window->RegisterPump(Engine::Pump);

		////////////////////////////////////////////////////////////////////////////////

		auto registry = ECS::Registry::GetInstance();

		pass_.resize(8);
		registry->Create(std::begin(pass_), std::end(pass_));
	}

	void Engine::Process(ECS::Registry& registry, Event::Resize&& resize)
	{
		//TODO: check Context on that
		renderer_->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		renderer_->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);

		swapchain_->Resize(resize.width_, resize.height_);
		return;
	}

	void Engine::Process(ECS::Registry& registry, Event::Create&&)
	{
		//TODO: move it out
		editor_ = std::make_unique<Component::Editor>();
		editor_->Create(renderer_);

		//TODO: make it local to Engine
		registry.GetView<Component::PassConcept>().Each
		(
			[this](const auto, Component::PassConcept& pass)
			{ 
				pass->Create(renderer_);
			}
		);

		registry.Sort<Root::Component::PassConcept>();
	}

	void Engine::Process(ECS::Registry& registry, Event::Process&&)
	{
		renderer_->BeginFrame();

		command_list_ = renderer_->GetNewCommandList();

		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				swapchain_->GetBackBufferResource(),
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			);

			command_list_->ResourceBarrier(1, &bb_barier);
		}

		editor_->Setup(renderer_, registry, pass_[0]);

		return;
	}

	void Engine::Process(ECS::Registry& registry, Event::Frame::Update&& update)
	{
		update.width_ = editor_->GetWidth();
		update.height_ = editor_->GetHeight();

		//TODO: to be removed
		registry.GetView<Component::PassConcept>().Each
		(
			[this, update](const auto, Component::PassConcept& pass)
			{ 
				pass->Update(update);
			}
		);
	}

	void Engine::Process(ECS::Registry& registry, Event::Frame::Begin&&)
	{
		registry.GetView<Component::PassConcept>().Each
		(
			[this](const auto, Component::PassConcept& pass) 
			{ 
				pass->Begin(command_list_);
			}
		);
	}

	void Engine::Process(ECS::Registry& registry, Event::Frame::Render&&)
	{
		auto project = editor_->GetProject();

		for (ECS::EntityType pass_id : pass_)
		{
			if (registry.Has<Component::PassConcept>(pass_id))
			{		
				auto& pass = registry.Get<Component::PassConcept>(pass_id);
				pass->Render();

				project->Draw(pass->GetContext());
			}
		}
	}

	void Engine::Process(ECS::Registry& registry, Event::Frame::End&&)
	{
		registry.GetView<Component::PassConcept>().Each
		(
			[this](const auto, Component::PassConcept& pass) 
			{ 
				pass->End(); 
			}
		);

		{	
			auto result = command_list_->Close();
			madX::Engine::ThrowIfFailed(result, "[Engine::Process(Event::End) at swapchain::wait] ID3D12GraphicsCommandList::Close failed");

			ID3D12CommandList* const lists[] = { command_list_ };
			renderer_->GetGraphicsQueue()->ExecuteCommandLists(1, lists);
		}

		swapchain_->Wait(renderer_->GetFrame());

		////////////////////////////////////////////////////////////////////////////////

		command_list_ = renderer_->GetNewCommandList();
		command_list_->OMSetRenderTargets(1, swapchain_->GetBackBufferHandle(), TRUE, NULL);

		float clear_color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		command_list_->ClearRenderTargetView(*swapchain_->GetBackBufferHandle(), clear_color, 0, NULL);
		
		editor_->Render(command_list_, registry, pass_[0]);

		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition(swapchain_->GetBackBufferResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			command_list_->ResourceBarrier(1, &bb_barier);
		}

		{
			auto result = command_list_->Close();
			madX::Engine::ThrowIfFailed(result, "[Engine::Process(Event::End) at swapchain::present] ID3D12GraphicsCommandList::Close failed");

			ID3D12CommandList* const lists[] = { command_list_ };
			renderer_->GetGraphicsQueue()->ExecuteCommandLists(1, lists);

			if (ImGuiIO& io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault(NULL, (void*)command_list_);
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		swapchain_->Present();

		//TODO: flush every frame for GPU Validation
		renderer_->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		renderer_->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);

		renderer_->EndFrame();
	}

	void Engine::Process(ECS::Registry& registry, Event::Terminate&& terminate)
	{
		//TODO: Kill 'Em All
		//TODO: improve orphans

		registry.GetView<Component::PassConcept>().Each
		(
			[this](const auto, Component::PassConcept& pass) 
			{
				pass->Destroy();
			}
		);

		editor_->Destroy();

		swapchain_.reset();
		renderer_.reset();

		if constexpr (madX::Engine::cEngineMode_Debug && !madX::Engine::cEngineMode_PIX)
		{
			HMODULE library = ::LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

			//TODO: error handling
			if (library)
			{
				typedef HRESULT(WINAPI * LPDXGIGETDEBUGINTERFACE)(REFIID, void**);
				auto debug = reinterpret_cast<LPDXGIGETDEBUGINTERFACE>(reinterpret_cast<void*>(::GetProcAddress(library, "DXGIGetDebugInterface")));

				IDXGIDebug* debug_controller;
				if (SUCCEEDED(debug(IID_PPV_ARGS(&debug_controller))))
				{
					auto result = debug_controller->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
					debug_controller->Release();
				}
			}
		}
	}
}