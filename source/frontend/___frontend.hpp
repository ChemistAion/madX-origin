import stl.core;
import microsoft.windows;
import microsoft.directx;
import microsoft.xmath;
import gui.imgui;

import madX.Backend;

namespace madX::Editor
{
	class Frontend final
	{
		friend class Window;

		////////////////////////////////////////////////////////////////////////////////
		//TODO: cleanup this

		//std::jthread worker_;
		std::atomic_uint64_t size_;

		struct Message
		{
			HWND hwnd_;
			UINT id_;
			WPARAM w_param_;
			LPARAM l_param_;
		};

		//TODO: lock-free spsc
		//std::mutex lock_;
		std::queue<Message> queue_;

		////////////////////////////////////////////////////////////////////////////////

	//	std::map<std::string, Panel::PanelBase> panels_;
	//	Editor::Processor processor_;
	//	
	//	//TODO: meta
	//	std::shared_ptr<Root::Component::Project> project_;

		void MessagePump();

	//	void Processor(std::stop_token token, madX::Editor::Window* window)
	//	{
	//		while (false == token.stop_requested())
	//		{
	//			Tick(window);
	//		}
	//	}

	public:
		explicit Frontend(HWND);
		~Frontend();

		void Create();

		void Update();
		void Begin();
		void Process();
		void End();

		//	void StartWorker(std::shared_ptr<madX::Editor::Window>& window)
		//	{
		//		worker_ = std::jthread(std::bind_front(&Backend::Processor, this), window.get());
		//	}

		void OnActivated() {};
		void OnDeactivated() {};
		void OnSuspending() {};
		void OnResuming() {};
		void OnWindowSizeChanged(std::uint32_t width, std::uint32_t height);
	};
}