#include "_editor.hpp"

constexpr decltype(auto) cEditorWindowTitle_Name = _T("madX::Editor");

int APIENTRY
WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE previousInstance, _In_ LPSTR commandLine, _In_ int showCommand)
{
	assert(instance == ::GetModuleHandle(NULL));

	UNREFERENCED_PARAMETER(previousInstance);
	UNREFERENCED_PARAMETER(commandLine);
	UNREFERENCED_PARAMETER(showCommand);

	auto logger = Core::Logger::GetInstance();
	logger->RegisterSink(std::make_unique<madX::Core::SinkMSVC>());
	logger->RegisterSink(std::make_unique<madX::Core::SinkSTD>());

	////////////////////////////////////////////////////////////////////////////////

	HANDLE process = ::GetCurrentProcess();

	::setlocale(LC_ALL, "");                      //TODO: result
	::CoInitializeEx(NULL, COINIT_MULTITHREADED); //TODO: result
	::SymInitialize(process, NULL, TRUE);         //TODO: result

	if constexpr (cmadXMode_Debug)
	{
		int debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

		debugFlag |= _CRTDBG_ALLOC_MEM_DF;
		debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
		debugFlag |= _CRTDBG_LEAK_CHECK_DF;

		_CrtSetDbgFlag(debugFlag);

		//auto mode = _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE;
		auto mode = _CRTDBG_MODE_DEBUG;

		_CrtSetReportMode(_CRT_WARN, mode);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);

		_CrtSetReportMode(_CRT_ERROR, mode);
		_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);

		_CrtSetReportMode(_CRT_ASSERT, mode);
		_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	}

	////////////////////////////////////////////////////////////////////////////////

	logger->Info("Editor::Initialize... begin");
	logger->Info("Current path: " + std::filesystem::current_path().string());

	Meta::Config config("config.json");

	//	if (config.IsEnabled("Console"))
	//	{
	//		auto console = madX::Core::Console::GetInstance();
	//	}

	//	auto window = madX::Core::Window::GetInstance();
	//	window->Create();
	
	//	auto handle = window->GetHandle();
	//	::SetWindowText(handle, cEditorWindowTitle_Name);

	auto registry = ECS::Registry::GetInstance();
	MSG msg = {};

	Core::Timer timer;

	try
	{
		const auto workers = config.GetValue<std::int64_t>("Workers");
		workers.second ? registry->Mount<Root::System::Worker>(workers.first) : registry->Mount<Root::System::Worker>();		

		//TODO: on Event::Create
		//	for (const auto& volume : config.GetNamesList("Volumes"))
		//	{
		//		registry->Emplace<Root::Component::Volume>(registry->Create(), volume);
		//	}
		
		const auto update = config.GetValue<double>("StorageUpdateRate");
		update.second ? registry->Mount<Root::System::Storage>(update.first) : registry->Mount<Root::System::Storage>();

		const auto project = config.GetValue<Meta::String>("Project");
		assert(project.second);

		////////////////////////////////////////////////////////////////////////////////

		//	auto engine = registry->Mount<Root::System::Engine>();	
		//	engine->RegisterPass(0, new Root::Component::Deferred());

		//TODO: ecs::Create
		//registry->Emplace<Root::Component::Project>(registry->Create(), engine->GetRenderer());

		////////////////////////////////////////////////////////////////////////////////

		//	registry->Process(Root::Event::Create{});

		//	//TODO: make it happen in Renderer, in one shot
		//	{	
		//		auto& renderer = engine->GetRenderer();			
		//		auto& upload = renderer->GetHeapUpload();
		//	
		//		renderer->UploadStaticData(upload.GetCommandList());
		//		upload.Flush(renderer.get());
		//		renderer->FreeUploadStaticHeap();
		//	}
	}
	catch (...)
	{
		logger->Critical("Editor::Init Unhandled exception");
		//goto Terminate; //TODO: forbidden???
	}

	logger->Info("Editor::Initialize... end");

	////////////////////////////////////////////////////////////////////////////////

	logger->Info("Editor... begin");

	for (;;)
	{
		try
		{
			timer.Update();
			registry->Process(Root::Event::Tick { timer.GetTotalTime(), timer.GetUpdateTime() } );

			//TODO: vectorize handles...
			DWORD result = ::MsgWaitForMultipleObjectsEx(0, NULL, 1, QS_ALLINPUT, MWMO_ALERTABLE | MWMO_INPUTAVAILABLE);
			if (WAIT_OBJECT_0 == result || WAIT_FAILED == result)
			{
				while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
				{
					if (WM_QUIT != msg.message) [[likely]]
					{
						::TranslateMessage(&msg);
						::DispatchMessage(&msg);

						continue;
					}

					goto Terminate;
				}
			}
			
			registry->Process(Root::Event::Process {});
			registry->Process(Root::Event::Frame::Update { timer.GetTotalTime(), timer.GetUpdateTime() });
			registry->Process(Root::Event::Frame::Begin{});
			registry->Process(Root::Event::Frame::Render{});
			registry->Process(Root::Event::Frame::End{}); //TODO: watch out! combine with render? 

			registry->Process(Root::Event::Tock{});
		}
		catch (...)
		{
			logger->Critical("Editor::Main Unhandled exception");
			break;
		}
	};

	////////////////////////////////////////////////////////////////////////////////

Terminate:
	logger->Info("Editor... end");

	logger->Info("Editor::Terminate... begin");

	try
	{
		registry->Process(Root::Event::Terminate{});
	}
	catch (...)
	{
		logger->Critical("Editor::Terminate Unhandled exception");
		//goto Terminate;
	}

	::CoUninitialize();
	::SymCleanup(process); //TODO: result

	logger->Info("Editor::Terminate... end");

	assert(WM_QUIT == msg.message);
	return static_cast<int>(msg.wParam);
}
