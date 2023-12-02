#include "_editor.hpp"

#include "node.hpp"

namespace Nodes
{
	void Node::BuildNode(const Meta::String& name, Meta::Nodes::NodeType type, ImColor color) noexcept
	{
		const bool inherit = type & Meta::Nodes::NodeFlag_Inherit;

		name_ = name;
		type_ = type;
		state_ |= inherit ? Nodes::NodeStateFlag_Default : Nodes::NodeStateFlag_Rooted;
		color_ = color;

		pos_ = ImVec2(0.0f, 0.0f);
			
		area_title_.Min = ImVec2(0.0f, 0.0f);
		area_title_.Max = ImGui::CalcTextSize(name.data());

		const auto separator = Nodes::NodesSpacing;
		const auto root = type & Meta::Nodes::NodeType_Root;

		if (false == root)
		{
			area_node_ = ImRect
			(
				0.0f,
				0.0f,
				area_title_.Max.x + (2.5f * separator),
				separator * (type & Meta::Nodes::NodeFlag_Component ? 2.0f : 3.0f)
			);
		
			area_out_ = ImRect
			(
				area_node_.Max.x,
				0.0f,
				area_node_.Max.x + (0.65f * separator),
				area_node_.Max.y
			);
		
			area_parent_ = ImRect
			(
				0.0f,
				0.65f * -separator,
				2.0f * separator,
				0.0f
			);
		}
		else
		{
			area_node_ = ImRect
			(
				0.0f,
				0.0f,
				separator * 5.0f,
				separator * 5.0f
			);
		
			area_out_ = {};
			area_parent_ = {};
		}

		area_body_ = area_node_;

		area_title_.Translate(area_node_.GetCenter() - area_title_.GetCenter());

		if (false == root)
		{
			if (type & Meta::Nodes::NodeFlag_Output)
			{		
				area_node_.Max.x = area_out_.Max.x;
			}
		}
	}

	void Node::Draw(ImDrawList* draw_list, ImVec2 offset, float scale, NodesState state) const
	{
		if (false == (state_ & Nodes::NodeStateFlag_Visible))
			return;

		const ImRect body = GetImRectOnConvas(pos_, area_body_, offset, scale);
		const ImRect parent = GetImRectOnConvas(pos_, area_parent_, offset, scale);
		const ImRect out = GetImRectOnConvas(pos_, area_out_, offset, scale);
		const ImRect node = GetImRectOnConvas(pos_, area_node_, offset, scale);

		const ImColor light_color(color_.Value.x * 1.5f, color_.Value.y * 1.5f, color_.Value.z * 1.5f, color_.Value.w);
		const ImColor dark_color(color_.Value.x * 0.5f, color_.Value.y * 0.5f, color_.Value.z * 0.5f, color_.Value.w);
		const ImColor ultra_color(light_color.Value.x * 1.5f, light_color.Value.y * 1.5f, light_color.Value.z * 1.5f, light_color.Value.w);
		const ImColor selected_color = ImColor(1.0f, 1.0f, 1.0f, 0.25f);

		////////////////////////////////////////////////////////////////////////////////

		const float thickness = scale * 2.0f;
		const float rounding = NodesRoundingRadius * scale;
		const bool tab_top = type_ & Meta::Nodes::NodeFlag_Inherit;
		const bool tab_right = type_ & Meta::Nodes::NodeFlag_Output;

		const auto root = type_ & Meta::Nodes::NodeType_Root;

		const ImDrawFlags body_corners =
		  (tab_top ? ImDrawCornerFlags_BotLeft : ImDrawCornerFlags_Left) | (tab_right ? ImDrawCornerFlags_BotLeft : ImDrawCornerFlags_Right);

		////////////////////////////////////////////////////////////////////////////////

		//draw shadow
		{
			auto shadow_color = state_ & NodeStateFlag_Processing ? ultra_color : dark_color;// ImColor(0.0f, 0.0f, 0.0f, 0.2f);
			shadow_color.Value.w = 0.25f; 
		 		
			ImRect node_outline = node;
			node_outline.Expand(scale);

			draw_list->AddRect(node_outline.Min, node_outline.Max, shadow_color, rounding, ImDrawCornerFlags_All, 4.0f * scale);
			draw_list->AddRect(node_outline.Min, node_outline.Max, shadow_color, rounding, ImDrawCornerFlags_All, 8.0f * scale);
		}

		////////////////////////////////////////////////////////////////////////////////
	
		//draw body fill
		if (tab_top)
			draw_list->AddRectFilled(parent.Min, parent.Max, state_ & NodeStateFlag_TabTop ? ultra_color : color_, rounding, ImDrawCornerFlags_Top);

		if (tab_right)
			draw_list->AddRectFilled(out.Min, out.Max, state_ & NodeStateFlag_TabRight ? ultra_color : color_, rounding, ImDrawCornerFlags_Right);

		draw_list->AddRectFilled(body.Min, body.Max, color_, rounding, body_corners);

		////////////////////////////////////////////////////////////////////////////////

		{
			const ImColor processing_color = state_ & NodeStateFlag_Processing ? ultra_color : dark_color;

			if (tab_top)
				//draw_list->AddRect(parent.Min, parent.Max, state_ & NodeStateFlag_TabTop ? ultra_color : light_color, rounding, ImDrawCornerFlags_Top, thickness);
				draw_list->AddRect(parent.Min, parent.Max, state_ & NodeStateFlag_TabTop ? color_ : processing_color, rounding, ImDrawCornerFlags_Top, thickness);
			
			if (tab_right)
				//draw_list->AddRect(out.Min, out.Max, state_ & NodeStateFlag_Processing ? ultra_color : light_color, rounding, ImDrawCornerFlags_Right, thickness);
				draw_list->AddRect(out.Min, out.Max, state_ & NodeStateFlag_TabRight ? color_ : processing_color, rounding, ImDrawCornerFlags_Right, thickness);

			draw_list->AddRect(body.Min, body.Max, state_ & NodeStateFlag_Processing ? ultra_color : light_color, rounding, body_corners, thickness);

			if (false == (type_ & Meta::Nodes::NodeFlag_Component))
			{
				ImRect body_inline = body;
				body_inline.Expand(-rounding);
			
				draw_list->AddRect(body_inline.Min, body_inline.Max, state_ & NodeStateFlag_Processing ? ultra_color : light_color, rounding, body_corners, scale);
			
				if (root)
				{
					body_inline.Expand(-rounding * 0.5f);
					draw_list->AddRect(body_inline.Min, body_inline.Max, state_ & NodeStateFlag_Processing ? ultra_color : light_color, rounding, body_corners, scale);
					body_inline.Expand(-rounding * 0.5f);
					draw_list->AddRect(body_inline.Min, body_inline.Max, state_ & NodeStateFlag_Processing ? ultra_color : light_color, rounding, body_corners, scale);
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////

	//	bool strikethrough = type_ & Meta::Nodes::NodeFlag_Inherit;
		if (type_ & Meta::Nodes::NodeFlag_Disabled || false == (state_ & NodeStateFlag_Rooted))
		{		
			IM_ASSERT(false == node.IsInverted());
		
			const float separation = 10.0f * scale;
		
			const ImColor lines_color
			(
				dark_color.Value.x,
				dark_color.Value.y,
				dark_color.Value.z,
				type_ & Meta::Nodes::NodeFlag_Disabled ? 1.0f : 0.35f
			);

			const float lines_thickness = 3.0f * scale;

			for (float line = separation; true; line += separation)
			{
				ImVec2 start = node.Min + ImVec2(0.0f, line);
				ImVec2 stop = node.Min + ImVec2(line, 0.0f);
		
				if (start.y > node.Max.y)
					start = ImVec2(start.x + (start.y - node.Max.y), node.Max.y);
		
				if (stop.x > node.Max.x)
					stop = ImVec2(node.Max.x, stop.y + (stop.x - node.Max.x));
		
				if (start.x > node.Max.x)
					break;
		
				if (stop.y > node.Max.y)
					break;
		
				draw_list->AddLine(start, stop, lines_color, lines_thickness);
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		const ImRect title = GetImRectOnConvas(pos_, area_title_, offset, scale);
		ImGui::SetCursorScreenPos(title.GetTL());
		ImGui::Text(name_.data());
		
		////////////////////////////////////////////////////////////////////////////////

		if (state_ & (Nodes::NodeStateFlag_Marked | Nodes::NodeStateFlag_Selected))
		{
			if (tab_top)
				draw_list->AddRectFilled(parent.Min, parent.Max, selected_color, rounding, ImDrawCornerFlags_Top);

			if (tab_right)
				draw_list->AddRectFilled(out.Min, out.Max, selected_color, rounding, ImDrawCornerFlags_Right);

			draw_list->AddRectFilled(body.Min, body.Max, selected_color, rounding, body_corners);
		}

		////////////////////////////////////////////////////////////////////////////////

		if (false == inputs_.empty())
		{
			ImVec2 input_pos = GetInputPos(node, rounding);
			const ImColor processing_color = state_ & NodeStateFlag_Processing ? ultra_color : light_color;

			for (const auto& input : inputs_)
			{
				draw_list->AddCircleFilled(input_pos, rounding, input.state_ & Nodes::InputStateFlag_Hovered ? ultra_color : color_, NodesInputDrawSegments);
				draw_list->AddCircle(input_pos, rounding, processing_color, NodesInputDrawSegments, scale);

				if (state_ & (Nodes::NodeStateFlag_Marked | Nodes::NodeStateFlag_Selected))
				{
					draw_list->AddCircleFilled(input_pos, rounding, selected_color, NodesInputDrawSegments);
				}

				input_pos += GetInputSeparation(rounding);
			}
		}
	}
}