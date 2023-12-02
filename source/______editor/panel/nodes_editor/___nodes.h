#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_internal.h"

namespace Nodes
{
	enum NodesConnectorStateFlag_
	{
		NodesConnectorStateFlag_Default	= 0,
		NodesConnectorStateFlag_Visible	= 1 << 0,
		NodesConnectorStateFlag_Hovered	= 1 << 1,
		NodesConnectorStateFlag_Consider	= 1 << 2,
		NodesConnectorStateFlag_Draging	= 1 << 3
	};

	enum NodesNodeStateFlag_
	{
		NodesNodeStateFlag_Default			= 0,
		NodesNodeStateFlag_Visible			= 1 << 0,
		NodesNodeStateFlag_Hovered			= 1 << 1,
		NodesNodeStateFlag_Marked			= 1 << 2,
		NodesNodeStateFlag_Selected		= 1 << 3,
		NodesNodeStateFlag_Collapsed		= 1 << 4,
		NodesNodeStateFlag_Disabled		= 1 << 5,
		NodesNodeStateFlag_Processing		= 1 << 6 
	};

	enum NodesState_
	{
		NodesState_Default	= 0,
		NodesState_HoveringNode,
		NodesState_HoveringInput,
		NodesState_HoveringOutput,
		NodesState_Draging,
		NodesState_DragingInput,
		NodesState_DragingOutput,
		NodesState_Selecting
	};

	////////////////////////////////////////////////////////////////////////////////

	typedef std::uint64_t NodesConnectorType;
	typedef std::uint64_t NodesNodeType;

	typedef unsigned int NodesConnectorState;
	typedef unsigned int NodesNodeState;

	typedef unsigned int NodesState;

	////////////////////////////////////////////////////////////////////////////////

	// connector text name heights factors
	constexpr float NodesConnectorDotDiameter = 0.7f; // connectors dot diameter
	constexpr float NodesConnectorDotPadding = 0.35f; // connectors dot left/right sides padding
	constexpr float NodesConnectorDistance = 1.5f; // vertical distance between connectors centers
	
	// title text name heights factors
	constexpr float NodesHSeparation = 1.7f; // extra horizontal separation inbetween IOs
	constexpr float NodesVSeparation = 1.5f; // total IOs area separation from title and node bottom edge
	constexpr float NodesTitleHight = 2.0f;

	struct NodesNode;
	struct NodesInput;
	struct NodesOutput;

	////////////////////////////////////////////////////////////////////////////////

	struct NodesInput
	{
		ImVec2 pos_;
		ImRect area_input_;
		ImRect area_name_;
		NodesConnectorType type_;
		NodesConnectorState state_;
		const char* name_;
		NodesNode* target_;
		NodesOutput* output_;

		inline void TranslateInput(ImVec2 delta)
		{
			pos_ += delta;
			area_input_.Translate(delta);
			area_name_.Translate(delta);
		}

		inline void DrawInput(ImDrawList* draw_list, ImVec2 offset, float scale, NodesState nodes_state, float rounding, NodesNodeState state, bool first) const
		{
			if (type_ == NULL)
				return;

			if (nodes_state != NodesState_Draging && state_ & NodesConnectorStateFlag_Hovered && false == (state_ & NodesConnectorStateFlag_Consider))
			{
				const ImColor color = target_ == NULL ? ImColor(0.0f, 0.0f, 1.0f, 0.5f) : ImColor(1.0f, 0.5f, 0.0f, 0.5f);
				draw_list->AddRectFilled((area_input_.Min * scale) + offset, (area_input_.Max * scale) + offset, color);//, first ? rounding : 0.0f);
			}

			if (state_ & (NodesConnectorStateFlag_Consider | NodesConnectorStateFlag_Draging))
			{
				draw_list->AddRectFilled((area_input_.Min * scale) + offset, (area_input_.Max * scale) + offset, ImColor(0.0f, 1.0f, 0.0f, 0.5f)); //, first ? rounding : 0.0f);
			}

			bool consider_fill = false;
			consider_fill |= bool(state_ & NodesConnectorStateFlag_Draging);
			consider_fill |= bool(state_ & NodesConnectorStateFlag_Hovered && state_ & NodesConnectorStateFlag_Consider);

			ImColor color = consider_fill ? ImColor(0.0f, 1.0f, 0.0f, 1.0f) : ImColor(1.0f, 1.0f, 1.0f, 1.0f);
					
			consider_fill |= bool(target_);

			if (consider_fill)
				draw_list->AddCircleFilled((pos_ * scale) + offset, (NodesConnectorDotDiameter * 0.5f) * area_name_.GetHeight() * scale, color, 3);
			else
				draw_list->AddCircle((pos_ * scale) + offset, (NodesConnectorDotDiameter * 0.5f) * area_name_.GetHeight() * scale, color, 3, 1.0f * scale);

			//if (false == first)
			{			
				ImGui::SetCursorScreenPos((area_name_.Min * scale) + offset);
				ImGui::Text(name_);
			}
		}

		NodesInput(const char* name, NodesConnectorType type, bool first)
		{
			type_ = type;
			state_ = NodesConnectorStateFlag_Default;
			target_ = NULL;
			output_ = NULL;
			name_ = name;

			area_name_.Min = ImVec2(0.0f, 0.0f);
			//area_name_.Max = first ? ImVec2(0.0f, ImGui::CalcTextSize(name).y) : ImGui::CalcTextSize(name);
			area_name_.Max = ImGui::CalcTextSize(name);

			area_input_.Min = ImVec2(0.0f, 0.0f);
			area_input_.Max.x = NodesConnectorDotPadding + NodesConnectorDotDiameter + NodesConnectorDotPadding;
			area_input_.Max.y = NodesConnectorDistance;
			area_input_.Max *= area_name_.GetHeight();

			ImVec2 offset = ImVec2(0.0f, 0.0f) - area_input_.GetCenter();

			area_name_.Translate(ImVec2(area_input_.GetWidth(), (area_input_.GetHeight() - area_name_.GetHeight()) * 0.5f));

			area_input_.Max.x += area_name_.GetWidth();
			area_input_.Max.x += NodesConnectorDotPadding * area_name_.GetHeight();

			area_input_.Translate(offset);
			area_name_.Translate(offset);	
		}
	};

	struct NodesOutput
	{
		ImVec2 pos_;
		ImRect area_output_;
		ImRect area_name_;
		NodesConnectorType type_;
		NodesConnectorState state_;
		const char* name_;
		unsigned int connections_;

		inline void TranslateOutput(ImVec2 delta)
		{
			pos_ += delta;
			area_output_.Translate(delta);
			area_name_.Translate(delta);
		}

		inline void DrawOutput(ImDrawList* draw_list, ImVec2 offset, float scale, NodesState nodes_state, float rounding, NodesNodeState state, bool first) const
		{
			if (type_ == NULL)
				return;

			if (nodes_state != NodesState_Draging && state_ & NodesConnectorStateFlag_Hovered && false == (state_ & NodesConnectorStateFlag_Consider))
			{
				draw_list->AddRectFilled((area_output_.Min * scale) + offset, (area_output_.Max * scale) + offset, ImColor(0.0f, 0.0f, 1.0f, 0.5f));//, first ? rounding : 0.0f);
			}

			if (state_ & (NodesConnectorStateFlag_Consider | NodesConnectorStateFlag_Draging))
			{
				draw_list->AddRectFilled((area_output_.Min * scale) + offset, (area_output_.Max * scale) + offset, ImColor(0.0f, 1.0f, 0.0f, 0.5f));//, first ? rounding : 0.0f);
			}

			bool consider_fill = false;
			consider_fill |= bool(state_ & NodesConnectorStateFlag_Draging);
			consider_fill |= bool(state_ & NodesConnectorStateFlag_Hovered && state_ & NodesConnectorStateFlag_Consider);

			ImColor color = consider_fill ? ImColor(0.0f, 1.0f, 0.0f, 1.0f) : ImColor(1.0f, 1.0f, 1.0f, 1.0f);

			consider_fill |= bool(connections_ > 0);

			if (consider_fill)
				draw_list->AddCircleFilled((pos_ * scale) + offset, (NodesConnectorDotDiameter * 0.5f) * area_name_.GetHeight() * scale, color, 3);
			else
				draw_list->AddCircle((pos_ * scale) + offset, (NodesConnectorDotDiameter * 0.5f) * area_name_.GetHeight() * scale, color, 3, 1.0f * scale);

			//if (false == first)
			{
				ImGui::SetCursorScreenPos((area_name_.Min * scale) + offset);
				ImGui::Text(name_);
			}
		}

		NodesOutput(const char* name, NodesConnectorType type, bool first)
		{
			type_ = type;
			state_ = NodesConnectorStateFlag_Default;
			connections_ = 0;
			name_ = name;

			//area_name_.Min = first ? ImVec2(0.0, -ImGui::CalcTextSize(name).y) : ImVec2(0.0f, 0.0f) - ImGui::CalcTextSize(name);
			area_name_.Min = ImVec2(0.0f, 0.0f) - ImGui::CalcTextSize(name);
			area_name_.Max = ImVec2(0.0f, 0.0f);

			area_output_.Min.x = NodesConnectorDotPadding + NodesConnectorDotDiameter + NodesConnectorDotPadding;
			area_output_.Min.y = NodesConnectorDistance;
			area_output_.Min *= -area_name_.GetHeight();
			area_output_.Max = ImVec2(0.0f, 0.0f);

			ImVec2 offset = ImVec2(0.0f, 0.0f) - area_output_.GetCenter();

			area_name_.Translate(ImVec2(area_output_.Min.x, (area_output_.GetHeight() - area_name_.GetHeight()) * -0.5f));

			area_output_.Min.x -= area_name_.GetWidth();
			area_output_.Min.x -= NodesConnectorDotPadding * area_name_.GetHeight();

			area_output_.Translate(offset);
			area_name_.Translate(offset);
		}
	};

	struct NodesNode
	{
		ImRect area_node_;
		ImRect area_name_;
		float title_height_;
		float body_height_;
		float collapsed_width_;
		NodesNodeState state_;
		NodesNodeType type_;
		const char* name_;
		ImColor color_;
		ImVector<NodesInput> inputs_;
		ImVector<NodesOutput> outputs_;

		inline void TranslateNode(ImVec2 delta, bool selected_only = false)
		{
			if (selected_only && !(state_ & NodesNodeStateFlag_Selected))
				return;

			area_node_.Translate(delta);
			area_name_.Translate(delta);

			for (int input_idx = 0; input_idx < inputs_.size(); ++input_idx)
				inputs_[input_idx].TranslateInput(delta);
		
			for (int output_idx = 0; output_idx < outputs_.size(); ++output_idx)
				outputs_[output_idx].TranslateOutput(delta);
		}

		inline void BuildNodeGeometry(ImVec2 inputs_size, ImVec2 outputs_size)
		{
			body_height_ = ImMax(inputs_size.y, outputs_size.y) + (NodesVSeparation * area_name_.GetHeight());

			collapsed_width_ = ImGui::CalcTextSize(name_).x;
			area_node_.Min = ImVec2(0.0f, 0.0f);
			area_node_.Max = ImVec2(0.0f, 0.0f);
			
			if (inputs_.empty() && outputs_.empty())
			{
				area_node_.Max.x += collapsed_width_;
				area_node_.Max.x += NodesHSeparation * area_name_.GetHeight();
				area_node_.Max.y = title_height_;
			}
			else
			{
				area_node_.Max.x += inputs_size.x + outputs_size.x;
				area_node_.Max.x += NodesHSeparation * area_name_.GetHeight();
				area_node_.Max.y += title_height_ + body_height_;		
			}
			
			area_name_.Translate(ImVec2((area_node_.GetWidth() - area_name_.GetWidth()) * 0.5f, ((title_height_ - area_name_.GetHeight()) * 0.5f)));
					
			ImVec2 inputs = area_node_.GetTL();
			inputs.y += title_height_ + (NodesVSeparation * area_name_.GetHeight() * 0.5f);
			for (int input_idx = 0; input_idx < inputs_.size(); ++input_idx)
			{
			//	if (input_idx == 0)
			//	{
			//		inputs_[input_idx].TranslateInput(area_node_.GetTL() - inputs_[input_idx].area_input_.GetTL());
			//		inputs_[input_idx].TranslateInput(ImVec2(0.0f, 0.5f * (title_height_ - inputs_[input_idx].area_input_.GetHeight())));
			//	}
			//	else
				{
					inputs_[input_idx].TranslateInput(inputs - inputs_[input_idx].area_input_.GetTL());
					inputs.y += inputs_[input_idx].area_input_.GetHeight();					
				}
			}
			
			ImVec2 outputs = area_node_.GetTR();
			outputs.y += title_height_ + (NodesVSeparation * area_name_.GetHeight() * 0.5f);
			for (int output_idx = 0; output_idx < outputs_.size(); ++output_idx)
			{

			//	if (output_idx == 0)
			//	{
			//		outputs_[output_idx].TranslateOutput(area_node_.GetTR() - outputs_[output_idx].area_output_.GetTR());
			//		outputs_[output_idx].TranslateOutput(ImVec2(0.0f, 0.5f * (title_height_ - outputs_[output_idx].area_output_.GetHeight())));
			//	}
			//	else
				{
					outputs_[output_idx].TranslateOutput(outputs - outputs_[output_idx].area_output_.GetTR());
					outputs.y += outputs_[output_idx].area_output_.GetHeight();
				}
			}
		}

		inline void DrawNode(ImDrawList* draw_list, ImVec2 offset, float scale, NodesState state) const
		{
			if (false == (state_ & NodesNodeStateFlag_Visible))
				return;

			ImRect node_rect = area_node_;
			node_rect.Min *= scale;
			node_rect.Max *= scale;
			node_rect.Translate(offset);

			float rounding = title_height_ * scale * 0.333f;

			ImColor head_color = color_, body_color = color_;
			head_color.Value.x *= 0.5;
			head_color.Value.y *= 0.5;
			head_color.Value.z *= 0.5;

			head_color.Value.w = 1.00f;
			body_color.Value.w = 0.75f;		

			const ImVec2 outline(4.0f * scale, 4.0f * scale);
			const ImDrawFlags rounding_corners_flags = ImDrawCornerFlags_All;
			const bool no_connectors = inputs_.empty() && outputs_.empty();

			if (state_ & NodesNodeStateFlag_Disabled)
			{
				body_color.Value.w = 0.25f;

				if (state_ & NodesNodeStateFlag_Collapsed)
					head_color.Value.w = 0.25f;
			}

			if (state_ & NodesNodeStateFlag_Processing)
				draw_list->AddRectFilled(node_rect.Min - outline, node_rect.Max + outline, body_color, rounding, rounding_corners_flags);
			else
				draw_list->AddRectFilled(node_rect.Min, node_rect.Max, body_color, rounding, rounding_corners_flags);

			if (false == no_connectors)
			{
				const ImVec2 head = node_rect.GetTR() + ImVec2(0.0f, title_height_ * scale);

				if (false == (state_ & NodesNodeStateFlag_Collapsed))
					draw_list->AddLine(ImVec2(node_rect.Min.x, head.y), ImVec2(head.x - 1.0f, head.y), ImColor(0.0f, 0.0f, 0.0f, 0.5f), 4.0f * scale);
		
				const ImDrawFlags head_corners_flags = ((state_ & NodesNodeStateFlag_Collapsed) || no_connectors) ? rounding_corners_flags : ImDrawCornerFlags_Top;
				draw_list->AddRectFilled(node_rect.Min, head, head_color, rounding, head_corners_flags);	
			}

			////////////////////////////////////////////////////////////////////////////////

			if (state_ & NodesNodeStateFlag_Disabled)
			{
				IM_ASSERT(false == node_rect.IsInverted());

				const float separation = 15.0f * scale;

				for (float line = separation; true; line += separation)
				{
					ImVec2 start = node_rect.Min + ImVec2(0.0f, line);
					ImVec2 stop = node_rect.Min + ImVec2(line, 0.0f);

					if (start.y > node_rect.Max.y)
						start = ImVec2(start.x + (start.y - node_rect.Max.y), node_rect.Max.y);

					if (stop.x > node_rect.Max.x)
						stop = ImVec2(node_rect.Max.x, stop.y + (stop.x - node_rect.Max.x));

					if (start.x > node_rect.Max.x)
						break;

					if (stop.y > node_rect.Max.y)
						break;

					draw_list->AddLine(start, stop, body_color, 3.0f * scale);
				}
			}

			////////////////////////////////////////////////////////////////////////////////

			if (false == (state_ & NodesNodeStateFlag_Collapsed))
			{
				for (int input_idx = 0; input_idx < inputs_.size(); ++input_idx)
					inputs_[input_idx].DrawInput(draw_list, offset, scale, state, rounding, state_, input_idx == 0);

				for (int output_idx = 0; output_idx < outputs_.size(); ++output_idx)
					outputs_[output_idx].DrawOutput(draw_list, offset, scale, state, rounding, state_, output_idx == 0);
			}
			//	else
			//	{
			//		if (false == inputs_.empty())
			//			inputs_[0].DrawInput(draw_list, offset, scale, state, rounding, state_, true);
			//		if (false == outputs_.empty())
			//			outputs_[0].DrawOutput(draw_list, offset, scale, state, rounding, state_, true);
			//	}

			////////////////////////////////////////////////////////////////////////////////

			ImGui::SetCursorScreenPos(((area_name_.Min + ImVec2(2, 2)) * scale) + offset);
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
			ImGui::Text(name_);
			ImGui::PopStyleColor();

			ImGui::SetCursorScreenPos((area_name_.Min * scale) + offset);
			ImGui::Text(name_);

			if (state_ & (NodesNodeStateFlag_Marked | NodesNodeStateFlag_Selected))
				draw_list->AddRectFilled(node_rect.Min, node_rect.Max, ImColor(1.0f, 1.0f, 1.0f, 0.25f), rounding, rounding_corners_flags);

			if (state_ & NodesNodeStateFlag_Processing)
			{
				ImColor processing_color = color_;
				processing_color.Value.x *= 1.5;
				processing_color.Value.y *= 1.5;
				processing_color.Value.z *= 1.5;
				processing_color.Value.w = 1.0f;

				draw_list->AddRect(node_rect.Min - outline, node_rect.Max + outline, processing_color, rounding, rounding_corners_flags, 2.0f * scale);		
			}
			else
			{
				draw_list->AddRect
				(
					node_rect.Min - outline * 0.5f,
					node_rect.Max + outline * 0.5f,
					ImColor(0.0f, 0.0f, 0.0f, 0.5f),
					rounding,
					rounding_corners_flags,
					3.0f * scale
				);		
			}
		}

		NodesNode(const char* name, NodesNodeType type, ImColor color)
		{
			name_ = name;
			type_ = type;
			state_ = NodesNodeStateFlag_Default;
			color_ = color;

			area_name_.Min = ImVec2(0.0f, 0.0f);
			area_name_.Max = ImGui::CalcTextSize(name);
			title_height_ = NodesTitleHight * area_name_.GetHeight();
		}
	};

	////////////////////////////////////////////////////////////////////////////////

	//NodesConnectionDesc size round up to 32 bytes to be cache boundaries friendly
	//constexpr int NodesNamesMaxLen = 32 - sizeof(NodesConnectorType);

	struct NodesConnectionDesc
	{
		const char* name_;//	[NodesNamesMaxLen];
		NodesConnectorType type_;
	};

	//TODO: ImVector me
	struct NodesNodeDesc
	{
		const char* name_;//	[NodesNamesMaxLen];
		NodesNodeType type_;
		ImColor color_;
		ImVector<NodesConnectionDesc> inputs_;
		ImVector<NodesConnectionDesc> outputs_;
	};

	////////////////////////////////////////////////////////////////////////////////

	struct Nodes
	{
	private:
		ImVec2 mouse_;
		ImVec2 pos_;
		ImVec2 size_;
		ImVec2 scroll_;
		ImVec4 connection_;
		float scale_;

		////////////////////////////////////////////////////////////////////////////////

		NodesState state_;

		ImRect area_;
		NodesNode* element_node_ = NULL;
		NodesInput* element_input_ = NULL;
		NodesOutput* element_output_ = NULL;
		NodesNode* processing_node_ = NULL;

		////////////////////////////////////////////////////////////////////////////////

		ImVector<NodesNode*> nodes_;
		ImVector<NodesNodeDesc> nodes_desc_;

		////////////////////////////////////////////////////////////////////////////////
	
	private:
		void UpdateCanvasGeometry(ImDrawList* draw_list);
		NodesNode* UpdateNodesFromCanvas();
		NodesNode* CreateNodeFromDesc(NodesNodeDesc* desc, ImVec2 pos);

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

		inline bool ConnectionMatrix(NodesNode* input_node, NodesNode* output_node, NodesInput* input, NodesOutput* output)
		{
			if (input->target_ && input->target_ == output_node)
				return false;
			
			if (input->type_ == output->type_)
				return true;
			
			return true;
		}

		inline bool SortSelectedNodesOrder();
	public:
		void Update();
		void ProcessNodes();
		void ProcessContextMenu();

		NodesNodeDesc& AddNodeDesc(NodesNodeDesc& desc)
		{
			nodes_desc_.push_back(desc);
			return nodes_desc_.back();
		}

		Nodes()
		{
			scale_ = 1.0f;
			state_ = NodesState_Default;
			element_node_ = NULL;
			element_input_ = NULL;
			element_output_ = NULL;
		}

		~Nodes()
		{
			for (int desc_idx = 0; desc_idx < nodes_desc_.size(); ++desc_idx)
			{
				NodesNodeDesc& node = nodes_desc_[desc_idx];

				node.inputs_.~ImVector();
				node.outputs_.~ImVector();
			}
			
			for (int node_idx = 0; node_idx < nodes_.size(); ++node_idx)
				delete nodes_[node_idx];
		}
	};

	////////////////////////////////////////////////////////////////////////////////
}