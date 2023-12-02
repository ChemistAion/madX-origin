#pragma once

#include "_editor.hpp"

#include "state.hpp"
#include "connector.hpp"
#include "node.hpp"

namespace madX::Editor::Panel
{
	class NodesEditor final : public PanelClass
	{
	private:
		float scale_ = 1.0f;
		Nodes::NodesState state_ = Nodes::NodesState_Default;

		Nodes::Node* element_node_ = nullptr;
		Nodes::ConnectorInput* element_input_ = nullptr;
		Nodes::ConnectorOutput* element_output_ = nullptr;

		Nodes::Node* processing_node_ = nullptr;
		//std::pair<ECS::EntityType, Nodes::Node*> processing_node_;

		ImRect area_;

		ImVec2 mouse_;
		ImVec2 pos_;
		ImVec2 size_;
		ImVec2 scroll_;
		ImVec4 connection_;

		////////////////////////////////////////////////////////////////////////////////

		Meta::Nodes::ShiftMap nodes_connector_abi_;
		Meta::Nodes::ShiftMap nodes_abi_;

		Meta::Nodes::NodeDescMap nodes_desc_map_;

		std::vector<Nodes::Node*> nodes_;

		////////////////////////////////////////////////////////////////////////////////

		void UpdateCanvasGeometry();
		Nodes::Node* UpdateNodesFromCanvas();

		inline bool ConnectionMatrix(Nodes::Node* input_node, Nodes::Node* output_node, Nodes::ConnectorInput* input, Nodes::ConnectorOutput* output)
		{
			if (input->target_ && input->target_ == output_node)
				return false;

			if (input->type_ == output->type_)
				return true;

			return true;
		}

		inline void DrawConnection(ImVec2 p1, ImVec2 p4, ImColor color)
		{
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			float line = 25.0f;

			ImVec2 p2 = p1;
			ImVec2 p3 = p4;

			p2 += (ImVec2(-line, 0.0f) * scale_);
			p3 += (ImVec2(+line, 0.0f) * scale_);

			draw_list->AddBezierCurve(p1, p2, p3, p4, color, 1.0f * scale_);
		}

		bool SortSelectedNodesOrder();

		Nodes::Node* CreateNode(const Meta::Nodes::ShiftMap& abi_map, const Meta::Nodes::NodeDesc& desc, ImVec2 pos);

		void Update();
		void ProcessNodes();
		void ProcessContextMenu();
	public:
		NodesEditor() noexcept;
		void RenderPanel(const std::shared_ptr<Root::Component::Project>&) override;
	};
}