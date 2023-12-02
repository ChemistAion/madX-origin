#include "_editor.hpp"

#include "backend/window.hpp"
#include "backend/backend.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"

constexpr decltype(auto) cEditorWindow_Name = _T("madX::Editor");
constexpr decltype(auto) cEditorWindowClass_Name = _T("madX::EditorClass");

int APIENTRY
WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE previousInstance, _In_ LPSTR commandLine, _In_ int showCommand)
{
	assert(instance == ::GetModuleHandle(NULL));

	UNREFERENCED_PARAMETER(previousInstance);
	UNREFERENCED_PARAMETER(commandLine);
	UNREFERENCED_PARAMETER(showCommand);

	//auto editor_log = Editor::Frontend::Log::GetInstance();
	auto logger = Core::Logger::GetInstance();
	logger->RegisterSink(std::make_unique<madX::Core::SinkMSVC>());
	logger->RegisterSink(std::make_unique<madX::Core::SinkSTD>());
	//logger->RegisterSink(std::make_unique<madX::Core::SinkImGUI>());

	//auto registry = ECS::Registry::GetInstance();

	////////////////////////////////////////////////////////////////////////////////

	HANDLE process = ::GetCurrentProcess();

	::setlocale(LC_ALL, "");                      //TODO: result
	////////::CoInitializeEx(NULL, COINIT_MULTITHREADED); 

	//_WIN32_WINNT >= 0x0A00
	//TODO: result
	//Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
	::Windows::Foundation::Initialize(RO_INIT_MULTITHREADED);

	::SymInitialize(process, NULL, TRUE);         //TODO: result
	
//	if constexpr (cMadXMode_Debug)
//	{
//		int debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
//	
//		debugFlag |= _CRTDBG_ALLOC_MEM_DF;
//		debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
//		debugFlag |= _CRTDBG_LEAK_CHECK_DF;
//	
//		_CrtSetDbgFlag(debugFlag);
//	
//		//auto mode = _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE;
//		auto mode = _CRTDBG_MODE_DEBUG;
//	
//		_CrtSetReportMode(_CRT_WARN, mode);
//		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
//	
//		_CrtSetReportMode(_CRT_ERROR, mode);
//		_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
//	
//		_CrtSetReportMode(_CRT_ASSERT, mode);
//		_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
//	}

	logger->Info("Current path: " + std::filesystem::current_path().string());

	////////////////////////////////////////////////////////////////////////////////

	logger->Info("Editor::Initialize... begin");

	auto window = std::make_shared<madX::Editor::Window>(cEditorWindowClass_Name, cEditorWindow_Name);

	auto backend = std::make_unique<madX::Editor::Backend>(window->GetHandle());
	backend->Create();
	
	//TODO: error handling
	::SetWindowLongPtr(window->GetHandle(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(backend.get()));

	logger->Info("Editor::Initialize... end");

	////////////////////////////////////////////////////////////////////////////////

	logger->Info("Editor... begin");

	MSG msg = {};

	//for (backend->StartWorker(window);;)
	for (;;)
	{
		try
		{
			//TODO: vectorize handles...
			//DWORD result = ::MsgWaitForMultipleObjectsEx(0, NULL, 1, QS_ALLINPUT, MWMO_ALERTABLE | MWMO_INPUTAVAILABLE);
			//if (WAIT_OBJECT_0 == result || WAIT_FAILED == result)
			{
				while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
				{
					if (WM_QUIT != msg.message) [[likely]]
					{
						::TranslateMessage(&msg);
						::DispatchMessage(&msg);

						continue;
					}

					logger->Info("Editor... end");
					goto Terminate;
				}
			}

			//backend->Tick(window.get());
			backend->Update();
			backend->Begin();
			backend->Process();
			backend->End();

		}
		catch (...)
		{
			logger->Critical("Editor::Main Unhandled exception");
			break;
		}
	};

	////////////////////////////////////////////////////////////////////////////////

Terminate:
	logger->Info("Editor::Terminate... begin");

	backend.reset();

	//	try
	//	{
	//		registry->Process(Root::Event::Terminate{});
	//	}
	//	catch (...)
	//	{
	//		logger->Critical("Editor::Terminate Unhandled exception");
	//		//goto Terminate;
	//	}

    //::DestroyWindow(hwnd);
	//::UnregisterClass(wc.lpszClassName, wc.hInstance);

	::Windows::Foundation::Uninitialize();
	////////::CoUninitialize();

	::SymCleanup(process); //TODO: result

	logger->Info("Editor::Terminate... end");

	assert(WM_QUIT == msg.message);
	return static_cast<int>(msg.wParam);
}






//Meta::Config config("config.json");
//auto registry = ECS::Registry::GetInstance();
//Core::Timer timer;

//	try
//	{
//		//const auto workers = config.GetValue<std::int64_t>("Workers");
//		//workers.second ? registry->Mount<Root::System::Worker>(workers.first) : registry->Mount<Root::System::Worker>();
//
//		//TODO: on Event::Create
//		//	for (const auto& volume : config.GetNamesList("Volumes"))
//		//	{
//		//		registry->Emplace<Root::Component::Volume>(registry->Create(), volume);
//		//	}
//
//		//const auto update = config.GetValue<double>("StorageUpdateRate");
//		//update.second ? registry->Mount<Root::System::Storage>(update.first) : registry->Mount<Root::System::Storage>();
//
//		//const auto project = config.GetValue<Meta::String>("Project");
//		//assert(project.second);
//
//		////////////////////////////////////////////////////////////////////////////////
//
//		//	auto engine = registry->Mount<Root::System::Engine>();
//		//	engine->RegisterPass(0, new Root::Component::Deferred());
//
//		//TODO: ecs::Create
//		//registry->Emplace<Root::Component::Project>(registry->Create(), engine->GetRenderer());
//
//		////////////////////////////////////////////////////////////////////////////////
//
//		//	registry->Process(Root::Event::Create{});
//
//		//	//TODO: make it happen in Renderer, in one shot
//		//	{
//		//		auto& renderer = engine->GetRenderer();
//		//		auto& upload = renderer->GetHeapUpload();
//		//
//		//		renderer->UploadStaticData(upload.GetCommandList());
//		//		upload.Flush(renderer.get());
//		//		renderer->FreeUploadStaticHeap();
//		//	}
//	}
//	catch (...)
//	{
//		logger->Critical("Editor::Init Unhandled exception");
//		//goto Terminate; //TODO: forbidden???
//	}