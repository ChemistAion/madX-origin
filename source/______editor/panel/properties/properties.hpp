#pragma once

#include "_editor.hpp"

namespace madX::Editor::Panel
{
	class Properties final : public PanelClass
	{
		PanelState panel_state_;
	public:
		Properties()
			: panel_state_("Properties")
		{
		}

		void ProcessPanel(const Editor::Processor&);

		const PanelState& GetPanelState() const noexcept
		{
			return panel_state_;
		}
	};
}


////	ImGui::Begin("Properties", NULL);
////	
////	//auto registry = ECS::Registry::GetInstance();
////	
////	//auto view = registry->GetView<Meta::Nodes::Node*>();
////	
////	//ImGui::Text("Registry size: %d", registry->Size());
////	//ImGui::Text("View status: %s", view.Empty() ? "empty" : "non-empty");
////	
////	ImGui::NewLine();
////	
////	//	view.Each
////	//	(
////	//	  [](auto&& entity, auto&& node)
////	//	  {
////	//			assert(node->type_ & Meta::Nodes::cNodeABI_TypeMask);
////	//			
////	//			const float size = ImGui::GetTextLineHeight();
////	//			const ImVec2 pos = ImGui::GetCursorScreenPos();
////	//			
////	//			ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size, pos.y + size), Nodes::GetImColorFromNodeType(node->type_));
////	//			ImGui::Dummy(ImVec2(size, size));
////	//			ImGui::SameLine();
////	//			ImGui::Text("%s, entity: %d", node->name_, entity);
////	//			return;
////	//	  }
////	//	);
////	
////	ImGui::End();