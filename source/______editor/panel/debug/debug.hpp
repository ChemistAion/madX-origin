#pragma once

#include "engine/texture.hpp"

namespace madX::Editor::Panel
{
	class Debug final : public PanelClass
	{
	public:
		struct Image
		{
			std::string name_;
			madX::Engine::TextureView* view_ = nullptr;
			std::uint32_t width_ = 0;
			std::uint32_t height_ = 0;
		};

		std::vector<Image> images_;
	private:
		std::unique_ptr<madX::Engine::Context> context_;
		std::shared_ptr<madX::Engine::Renderer> renderer_;

		PanelState panel_state_;
	public:
		Debug(const std::shared_ptr<madX::Engine::Renderer>& renderer)
			: panel_state_("Debug")
		{
			renderer_ = renderer;
			//context_ = std::make_unique<madX::Engine::Context>(renderer.get());
		}

		void ProcessPanel(const Editor::Processor&);

		const PanelState& GetPanelState() const noexcept
		{
			return panel_state_;
		}
	};
}