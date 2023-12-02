#include "_editor.hpp"

#include "state.hpp"
#include "connector.hpp"

namespace Nodes
{
//	ConnectorInput::ConnectorInput(const char* name, Meta::Nodes::ConnectorType type)
//	{
//		type_ = type;
//		state_ = ConnectorStateFlag_Default;
//		target_ = NULL;
//		output_ = NULL;
//		name_ = name;
//	
//		area_name_.Min = ImVec2(0.0f, 0.0f);
//		area_name_.Max = ImGui::CalcTextSize(name);
//	
//		area_connector_.Min = ImVec2(0.0f, 0.0f);
//		area_connector_.Max.x = cConnectorDotPadding + cConnectorDotDiameter + cConnectorDotPadding;
//		area_connector_.Max.y = cConnectorDistance;
//		area_connector_.Max *= area_name_.GetHeight();
//	
//		ImVec2 offset = ImVec2(0.0f, 0.0f) - area_connector_.GetCenter();
//	
//		area_name_.Translate(ImVec2(area_connector_.GetWidth(), (area_connector_.GetHeight() - area_name_.GetHeight()) * 0.5f));
//	
//		area_connector_.Max.x += area_name_.GetWidth();
//		area_connector_.Max.x += cConnectorDotPadding * area_name_.GetHeight();
//	
//		area_connector_.Translate(offset);
//		area_name_.Translate(offset);
//	}
//	
//	ConnectorOutput::ConnectorOutput(const char* name, Meta::Nodes::ConnectorType type)
//	{
//		type_ = type;
//		state_ = ConnectorStateFlag_Default;
//		connections_ = 0;
//		name_ = name;
//	
//		area_name_.Min = ImVec2(0.0f, 0.0f) - ImGui::CalcTextSize(name);
//		area_name_.Max = ImVec2(0.0f, 0.0f);
//	
//		area_connector_.Min.x = cConnectorDotPadding + cConnectorDotDiameter + cConnectorDotPadding;
//		area_connector_.Min.y = cConnectorDistance;
//		area_connector_.Min *= -area_name_.GetHeight();
//		area_connector_.Max = ImVec2(0.0f, 0.0f);
//	
//		ImVec2 offset = ImVec2(0.0f, 0.0f) - area_connector_.GetCenter();
//	
//		area_name_.Translate(ImVec2(area_connector_.Min.x, (area_connector_.GetHeight() - area_name_.GetHeight()) * -0.5f));
//	
//		area_connector_.Min.x -= area_name_.GetWidth();
//		area_connector_.Min.x -= cConnectorDotPadding * area_name_.GetHeight();
//	
//		area_connector_.Translate(offset);
//		area_name_.Translate(offset);
//	}
//	
//	////////////////////////////////////////////////////////////////////////////////
//	
//	void ConnectorInput::Draw(ImDrawList* draw_list, ImVec2 offset, float scale, NodesState nodes_state, float rounding, NodeState state) const
//	{
//		if (type_ == NULL)
//			return;
//	
//		if (nodes_state != NodesState_Draging && state_ & ConnectorStateFlag_Hovered && false == (state_ & ConnectorStateFlag_Consider))
//		{
//			const ImColor color = target_ == NULL ? ImColor(0.0f, 0.0f, 1.0f, 0.5f) : ImColor(1.0f, 0.5f, 0.0f, 0.5f);
//			draw_list->AddRectFilled((area_connector_.Min * scale) + offset, (area_connector_.Max * scale) + offset, color); //, first ? rounding : 0.0f);
//		}
//	
//		if (state_ & (ConnectorStateFlag_Consider | ConnectorStateFlag_Draging))
//		{
//			draw_list->AddRectFilled((area_connector_.Min * scale) + offset, (area_connector_.Max * scale) + offset,
//			  ImColor(0.0f, 1.0f, 0.0f, 0.5f)); //, first ? rounding : 0.0f);
//		}
//	
//		bool consider_fill = false;
//		consider_fill |= bool(state_ & ConnectorStateFlag_Draging);
//		consider_fill |= bool(state_ & ConnectorStateFlag_Hovered && state_ & ConnectorStateFlag_Consider);
//	
//		ImColor color = consider_fill ? ImColor(0.0f, 1.0f, 0.0f, 1.0f) : ImColor(1.0f, 1.0f, 1.0f, 1.0f);
//	
//		consider_fill |= bool(target_);
//	
//		if (consider_fill)
//			draw_list->AddCircleFilled((pos_ * scale) + offset, (cConnectorDotDiameter * 0.5f) * area_name_.GetHeight() * scale, color, 14);
//		else
//			draw_list->AddCircle((pos_ * scale) + offset, (cConnectorDotDiameter * 0.5f) * area_name_.GetHeight() * scale, color, 14, 1.0f * scale);
//	
//		ImGui::SetCursorScreenPos((area_name_.Min * scale) + offset);
//		ImGui::Text(name_);
//	}
//	
//	void ConnectorOutput::Draw(ImDrawList* draw_list, ImVec2 offset, float scale, NodesState nodes_state, float rounding, NodeState state) const
//	{
//		if (type_ == NULL)
//			return;
//	
//		if (nodes_state != NodesState_Draging && state_ & ConnectorStateFlag_Hovered && false == (state_ & ConnectorStateFlag_Consider))
//		{
//			draw_list->AddRectFilled((area_connector_.Min * scale) + offset, (area_connector_.Max * scale) + offset,
//			  ImColor(0.0f, 0.0f, 1.0f, 0.5f)); //, first ? rounding : 0.0f);
//		}
//	
//		if (state_ & (ConnectorStateFlag_Consider | ConnectorStateFlag_Draging))
//		{
//			draw_list->AddRectFilled((area_connector_.Min * scale) + offset, (area_connector_.Max * scale) + offset,
//			  ImColor(0.0f, 1.0f, 0.0f, 0.5f)); //, first ? rounding : 0.0f);
//		}
//	
//		bool consider_fill = false;
//		consider_fill |= bool(state_ & ConnectorStateFlag_Draging);
//		consider_fill |= bool(state_ & ConnectorStateFlag_Hovered && state_ & ConnectorStateFlag_Consider);
//	
//		ImColor color = consider_fill ? ImColor(0.0f, 1.0f, 0.0f, 1.0f) : ImColor(1.0f, 1.0f, 1.0f, 1.0f);
//	
//		consider_fill |= bool(connections_ > 0);
//	
//		if (consider_fill)
//			draw_list->AddCircleFilled((pos_ * scale) + offset, (cConnectorDotDiameter * 0.5f) * area_name_.GetHeight() * scale, color, 14);
//		else
//			draw_list->AddCircle((pos_ * scale) + offset, (cConnectorDotDiameter * 0.5f) * area_name_.GetHeight() * scale, color, 14, 1.0f * scale);
//	
//		ImGui::SetCursorScreenPos((area_name_.Min * scale) + offset);
//		ImGui::Text(name_);
//	}
}