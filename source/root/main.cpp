import stl.core;
import stl.memory;
import stl.filesystem;

import microsoft.windows;

extern "C" { _declspec(dllexport) extern const UINT D3D12SDKVersion = 608; }
extern "C" { _declspec(dllexport) extern const char* D3D12SDKPath = ".\\"; }

import microsoft.directx;

import madX.Core.Logger;
import madX.Platform.Window;
//import madX.Backend.Factory;
import madX.Project;

import madX.Editor;

#include "resources.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
{
	assert(instance == ::GetModuleHandle(NULL));

	//singletons
	std::shared_ptr<madX::Core::Logger> logger;
	std::shared_ptr<madX::Platform::Window> window;
	std::shared_ptr<madX::Backend::Factory> backend;
	std::shared_ptr<madX::Project> project;

	////////////////////////////////////////////////////////////////////////////////

	logger = std::move(madX::Core::Logger::GetInstance());
	logger->RegisterSink(std::make_unique<madX::Core::SinkMSVC>());
	//logger->RegisterSink(std::make_unique<madX::Core::SinkSTD>());
	//logger->RegisterSink(std::make_unique<madX::Core::SinkImGUI>());

	logger->Info("madX::begin");

	std::unique_ptr<madX::Editor::EditorUI> editor;

	////////////////////////////////////////////////////////////////////////////////

	MSG msg = {};
	Microsoft::WRL::Wrappers::RoInitializeWrapper wrl(RO_INIT_MULTITHREADED);

	if (HRESULT result = wrl; result != S_OK)
	{
		logger->Critical("WRL::RO_INIT_MULTITHREADED error=" + std::to_string(result));
		goto Terminate;
	}

	//	HANDLE process = ::GetCurrentProcess();
	//	::SymInitialize(process, NULL, TRUE);

	////////////////////////////////////////////////////////////////////////////////

	try
	{
		logger->Info("madX::pre-init");
		logger->Info("workdir path: " + std::filesystem::current_path().string());

		window = std::move(madX::Platform::Window::GetInstance());
		window->Create();

		backend = std::move(madX::Backend::Factory::GetInstance());
		//TODO: backend->enumerate adapters

		project = std::move(madX::Project::GetInstance());

		//TODO: modules creation
		//	core/log/hal
		//	assets/types
		//  input/output
		//	run-time
		//		anim
		//		audio
		//		cam
		//		ligths
		//		phys
		//		blue-prints
		//	engine
		//	ecs
		//	project
	}
	catch (...)
	{
		logger->Critical("madX::main unhandled exception");
		goto Terminate;
	}

	////////////////////////////////////////////////////////////////////////////////

	try
	{
		//TODO: modules startup
		logger->Info("madX::init");
	
		editor = std::make_unique<madX::Editor::EditorUI>();
		editor->Create();
	
		//window->RegisterPump(ImGui_ImplWin32_WndProcHandler);
	}
	catch (...)
	{
		logger->Critical("madX::main unhandled exception");
		goto Terminate;
	}

	////////////////////////////////////////////////////////////////////////////////

	logger->Info("madX::start");

	for (;;)
	{
		try
		{
			//	DWORD result = ::MsgWaitForMultipleObjectsEx(0, NULL, 1, QS_ALLINPUT, MWMO_ALERTABLE | MWMO_INPUTAVAILABLE);
			//	if (WAIT_OBJECT_0 == result || WAIT_FAILED == result)
			{
				while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
				{
					if (WM_QUIT != msg.message) [[likely]]
					{
						::TranslateMessage(&msg);
						::DispatchMessage(&msg);

						continue;
					}

					//TODO: consider this as too hard
					logger->Info("madX::quit");
					goto Terminate;
				}
			}

		editor->Update();
		editor->Begin();
		editor->Process();
		editor->End();

		}
		catch (...)
		{
			logger->Critical("madX::main unhandled exception");
			break;
		}
	};

	////////////////////////////////////////////////////////////////////////////////

Terminate:
	logger->Info("madX::terminate");

	//	//::DestroyWindow(hwnd);
	//	//::UnregisterClass(wc.lpszClassName, wc.hInstance);

	wrl.~wrl();

	//	::SymCleanup(process); //TODO: result

	////////////////////////////////////////////////////////////////////////////////

	logger->Info("madX::done");
	//TODO: modules shutdown

	//TODO: save log results

	assert(WM_QUIT == msg.message);
	return static_cast<int>(msg.wParam);
}


