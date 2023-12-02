import stl.core;
import microsoft.windows;
import microsoft.directx;
import microsoft.xmath;
import madX.Backend;
import madX.Frontend;
import madX.Framegraph;

import gui.imgui;

import madX.Platform.Window;

namespace madX::Editor
{
	class EditorUI : public Frontend
	{
		std::atomic_uint64_t size_;

		std::shared_ptr<madX::Backend::Processor> processor_;
		std::shared_ptr<madX::Backend::SwapChain> swapchain_;
		std::shared_ptr<madX::Backend::Pipeline> pipe_imgui_;
		std::shared_ptr<madX::Backend::Pipeline> pipe_raw_;
		std::unique_ptr<madX::Backend::Context> context_;

		Backend::HeapDescriptor::Element element_;

		////////////////////////////////////////////////////////////////////////////////
	public:
		EditorUI();

		~EditorUI();

		void Create() override;

		void Update() override;
		void Begin() override;
		void Process() override;
		void End() override;
		void Destroy() override;
	};
}