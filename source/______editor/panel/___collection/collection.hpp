#pragma once

#include "_editor.hpp"

namespace madX::Editor::Panel
{
	class Collection final : public PanelClass
	{
	public:
		void RenderPanel(const std::shared_ptr<Root::Component::Project>& project) override;
	};
}