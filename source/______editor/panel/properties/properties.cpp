#include "_editor.hpp"

namespace madX::Editor::Panel
{
	void Properties::ProcessPanel(const Editor::Processor& processor)
	{
		ImGui::Begin("Properties", NULL);
		
		////////////////////////////////////////////////////////////////////////////////
	
		auto print_node = [](std::size_t depth, auto&& node)
		{
			assert(node->type_ & Meta::Nodes::cNodeABI_TypeMask);
	
			const float size = ImGui::GetTextLineHeight();
			const ImVec2 pos = ImGui::GetCursorScreenPos() + ImVec2(size * depth, 0);
	
			ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size, pos.y + size), Meta::Nodes::GetImColorFromNodeType(node->type_));
			ImGui::Dummy(ImVec2(size * (depth + 1), size));
			ImGui::SameLine();
			//ImGui::Text("%*s%s", depth, "", node->name_.data());
			ImGui::Text("%s", node->name_.data());
			return;
		};
	
		////////////////////////////////////////////////////////////////////////////////
	
		const auto& linear_root = processor.GetLinearRoot();
		const auto processing_node = processor.GetProcessingNode();
	
		ImGui::Text("Nodes size: %d", linear_root.size());
		ImGui::NewLine();
	
		ImGui::Text("Processing node: ");
		ImGui::SameLine();
		if (processing_node)
			print_node(0, processing_node);
		else
			ImGui::Text("NONE");
	
		for (const auto& element : linear_root)
			print_node(element.depth_, element.node_);
	
		ImGui::End();
	}
}