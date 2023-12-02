#include "_editor.hpp"

#include "nodes.h"

namespace Nodes
{
	void Nodes::UpdateCanvasGeometry(ImDrawList* draw_list)
	{
		const ImGuiIO& io = ImGui::GetIO();

		mouse_ = ImGui::GetMousePos();

		{
			ImVec2 min = ImGui::GetWindowContentRegionMin();
			ImVec2 max = ImGui::GetWindowContentRegionMax();

			pos_ = ImGui::GetWindowPos() + min;
			size_ = max - min;	
		}

		ImRect canvas(pos_, pos_ + size_);

		////////////////////////////////////////////////////////////////////////////////

		if (ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_Home]))
		{
			scroll_ = {};
			scale_ = 1.0f;
		}
				
		////////////////////////////////////////////////////////////////////////////////

		if (false == ImGui::IsMouseDown(0) && canvas.Contains(mouse_))
		{
			if (ImGui::IsMouseDragging(1))
				scroll_ += io.MouseDelta;

			if (io.KeyShift && !io.KeyCtrl)
				scroll_.x += io.MouseWheel * 16.0f;

			if (!io.KeyShift && !io.KeyCtrl)
				scroll_.y += io.MouseWheel * 16.0f;

			if (!io.KeyShift && io.KeyCtrl)
			{
				ImVec2 focus = (mouse_ - scroll_ - pos_) / scale_;

				if (io.MouseWheel < 0.0f)
					for (float zoom = io.MouseWheel; zoom < 0.0f; zoom += 1.0f)
						scale_ = ImMax(0.3f, scale_ / 1.05f);

				if (io.MouseWheel > 0.0f)
					for (float zoom = io.MouseWheel; zoom > 0.0f; zoom -= 1.0f)
						scale_ = ImMin(3.0f, scale_ * 1.05f);

				ImVec2 shift = scroll_ + (focus * scale_);
				scroll_ += mouse_ - shift - pos_;
			}

			if (ImGui::IsMouseReleased(1) && element_node_ == NULL)
				if (io.MouseDragMaxDistanceSqr[1] < (io.MouseDragThreshold * io.MouseDragThreshold))
				{
					bool selected = false;

					for (int node_idx = 0; node_idx < nodes_.size(); ++node_idx)
					{
						if (nodes_[node_idx]->state_ & NodesNodeStateFlag_Selected)
						{					
							selected = true;
							break;
						}
					}

					if (false == selected)
						ImGui::OpenPopup("NodesContextMenu");
				}
		}

		////////////////////////////////////////////////////////////////////////////////

		const float grid = 64.0f * scale_;
		
		int mark_x = (int)(scroll_.x / grid);
		for (float x = std::fmodf(scroll_.x, grid); x < size_.x; x += grid, --mark_x)
		{		
			ImColor color = mark_x % 5 ? ImColor(0.5f, 0.5f, 0.5f, 0.1f) : ImColor(1.0f, 1.0f, 1.0f, 0.1f);
			draw_list->AddLine(ImVec2(x, 0.0f) + pos_, ImVec2(x, size_.y) + pos_, color, 0.1f);	
		}
			
		int mark_y = (int)(scroll_.y / grid);
		for (float y = std::fmodf(scroll_.y, grid); y < size_.y; y += grid, --mark_y)
		{
			ImColor color = mark_y % 5 ? ImColor(0.5f, 0.5f, 0.5f, 0.1f) : ImColor(1.0f, 1.0f, 1.0f, 0.1f);
			draw_list->AddLine(ImVec2(0.0f, y) + pos_, ImVec2(size_.x, y) + pos_, color, 0.1f);
		}
	}

	NodesNode* Nodes::UpdateNodesFromCanvas()
	{
		if (nodes_.empty())
			return NULL;

		const ImGuiIO& io = ImGui::GetIO();

		ImVec2 offset = pos_ + scroll_;
		ImRect canvas(pos_, pos_ + size_);
		NodesNode* hovered_node = NULL;

		for (int node_idx = nodes_.size(); node_idx != 0;)
		{
			NodesNode* node = nodes_[--node_idx];
			IM_ASSERT(node);

			ImRect node_rect = node->area_node_;
			node_rect.Min *= scale_;
			node_rect.Max *= scale_;
			node_rect.Translate(offset);

			node_rect.ClipWith(canvas);

			if (canvas.Overlaps(node_rect))
			{
				node->state_ |= NodesNodeStateFlag_Visible;
				node->state_ &= ~NodesNodeStateFlag_Hovered;
			}
			else
			{
				node->state_ &= ~(NodesNodeStateFlag_Visible | NodesNodeStateFlag_Hovered | NodesNodeStateFlag_Marked);
				continue;
			}

			if (NULL == hovered_node && node_rect.Contains(mouse_))
				hovered_node = node;

			////////////////////////////////////////////////////////////////////////////////

			if (state_ == NodesState_Selecting)
			{
				if (io.KeyCtrl && area_.Overlaps(node_rect))
				{
					node->state_ |= NodesNodeStateFlag_Marked;
					continue;
				}

				if (false == io.KeyCtrl && area_.Contains(node_rect))
				{
					node->state_ |= NodesNodeStateFlag_Marked;
					continue;
				}

				node->state_ &= ~NodesNodeStateFlag_Marked;
			}

			////////////////////////////////////////////////////////////////////////////////

			for (int input_idx = 0; input_idx < node->inputs_.size(); ++input_idx)
			{
				NodesInput& input = node->inputs_[input_idx];

				if (input.type_ == NULL)
					continue;

				input.state_ &= ~(NodesConnectorStateFlag_Hovered | NodesConnectorStateFlag_Consider | NodesConnectorStateFlag_Draging);

				if (state_ == NodesState_DragingInput)
				{
					if (&input == element_input_)
						input.state_ |= NodesConnectorStateFlag_Draging;

					continue;
				}				

				if (state_ == NodesState_DragingOutput)
				{
					if (element_node_ == node)
						continue;

					if (ConnectionMatrix(node, element_node_, &input, element_output_))
						input.state_ |= NodesConnectorStateFlag_Consider;
				}

				if (!hovered_node || hovered_node != node)
					continue;

				if (state_ == NodesState_Selecting)
					continue;

				if (state_ != NodesState_DragingOutput && node->state_ & NodesNodeStateFlag_Selected)
					continue;

				ImRect input_rect = input.area_input_;
				input_rect.Min *= scale_;
				input_rect.Max *= scale_;
				input_rect.Translate(offset);

				if (input_rect.Contains(mouse_))
				{
					if (state_ != NodesState_DragingOutput)
					{
						input.state_ |= NodesConnectorStateFlag_Hovered;
						continue;
					}
					
					if (input.state_ & NodesConnectorStateFlag_Consider)
						input.state_ |= NodesConnectorStateFlag_Hovered;
				}				
			}

			////////////////////////////////////////////////////////////////////////////////
			
			for (int output_idx = 0; output_idx < node->outputs_.size(); ++output_idx)
			{
				NodesOutput& output = node->outputs_[output_idx];

				if (output.type_ == NULL)
					continue;

				output.state_ &= ~(NodesConnectorStateFlag_Hovered | NodesConnectorStateFlag_Consider | NodesConnectorStateFlag_Draging);

				if (state_ == NodesState_DragingOutput)
				{
					if (&output == element_output_)
						output.state_ |= NodesConnectorStateFlag_Draging;

					continue;
				}

				if (state_ == NodesState_DragingInput)
				{
					if (element_node_ == node)
						continue;

					if (ConnectionMatrix(element_node_, node, element_input_, &output))
						output.state_ |= NodesConnectorStateFlag_Consider;
				}

				if (!hovered_node || hovered_node != node)
					continue;
			
				if (state_ == NodesState_Selecting)
					continue;

				if (state_ != NodesState_DragingInput && node->state_ & NodesNodeStateFlag_Selected)
					continue;
				
				ImRect output_rect = output.area_output_;
				output_rect.Min *= scale_;
				output_rect.Max *= scale_;
				output_rect.Translate(offset);

				if (output_rect.Contains(mouse_))
				{
					if (state_ != NodesState_DragingInput)
					{
						output.state_ |= NodesConnectorStateFlag_Hovered;
						continue;
					}

					if (output.state_ & NodesConnectorStateFlag_Consider)
						output.state_ |= NodesConnectorStateFlag_Hovered;
				}
			}	
		}

		if (hovered_node)
			hovered_node->state_ |= NodesNodeStateFlag_Hovered;

		return hovered_node;
	}

	NodesNode* Nodes::CreateNodeFromDesc(NodesNodeDesc* desc, ImVec2 pos)
	{
		IM_ASSERT(desc);
		NodesNode* node = new NodesNode(desc->name_, desc->type_, desc->color_);
		
		ImVec2 inputs;
		ImVec2 outputs;

		////////////////////////////////////////////////////////////////////////////////

		for (int input_idx = 0; input_idx < desc->inputs_.size(); ++input_idx)
		{
			bool first = input_idx == 0;
			NodesInput input(desc->inputs_[input_idx].name_, desc->inputs_[input_idx].type_, first);

			//if (false == first)
			{		
				inputs.x = ImMax(inputs.x, input.area_input_.GetWidth());
				inputs.y += input.area_input_.GetHeight();
			}

			node->inputs_.push_back(input);
		}

		for (int output_idx = 0; output_idx < desc->outputs_.size(); ++output_idx)
		{
			bool first = output_idx == 0;
			NodesOutput output(desc->outputs_[output_idx].name_, desc->outputs_[output_idx].type_, first);
		
			//if (false == first)
			{
				outputs.x = ImMax(outputs.x, output.area_output_.GetWidth());
				outputs.y += output.area_output_.GetHeight();
			}

			node->outputs_.push_back(output);
		}

		////////////////////////////////////////////////////////////////////////////////
		
		node->BuildNodeGeometry(inputs, outputs);
		node->TranslateNode(pos - node->area_node_.GetCenter());
		node->state_ |= NodesNodeStateFlag_Visible | NodesNodeStateFlag_Hovered | NodesNodeStateFlag_Processing;

		////////////////////////////////////////////////////////////////////////////////

		if (processing_node_)
			processing_node_->state_ &= ~(NodesNodeStateFlag_Processing);

		return processing_node_ = node;
	}

	bool Nodes::SortSelectedNodesOrder()
	{
		bool selected = false;

		ImVector<NodesNode*> nodes_unselected;
		nodes_unselected.reserve(nodes_.size());

		ImVector<NodesNode*> nodes_selected;
		nodes_selected.reserve(nodes_.size());

		for (NodesNode** iterator = nodes_.begin(); iterator != nodes_.end(); ++iterator)
		{
			NodesNode* node = ((NodesNode*)*iterator);

			if (node->state_ & NodesNodeStateFlag_Marked || node->state_ & NodesNodeStateFlag_Selected)
			{
				selected = true;
				node->state_ &= ~NodesNodeStateFlag_Marked;
				node->state_ |= NodesNodeStateFlag_Selected;
				nodes_selected.push_back(node);
			}
			else
				nodes_unselected.push_back(node);
		}

		int node_idx = 0;

		for (int unselected_idx = 0; unselected_idx < nodes_unselected.size(); ++unselected_idx)
			nodes_[node_idx++] = nodes_unselected[node_idx];

		for (int selected_idx = 0; selected_idx < nodes_selected.size(); ++selected_idx)
			nodes_[node_idx++] = nodes_selected[selected_idx];

		return selected;
	}

	void Nodes::Update()
	{
		const ImGuiIO& io = ImGui::GetIO();
		
		UpdateCanvasGeometry(ImGui::GetWindowDrawList());

		////////////////////////////////////////////////////////////////////////////////

		NodesNode* hovered_node = UpdateNodesFromCanvas();

		bool consider_hover = state_ == NodesState_Default;
		consider_hover |= state_ == NodesState_HoveringNode;
		consider_hover |= state_ == NodesState_HoveringInput;
		consider_hover |= state_ == NodesState_HoveringOutput;

		////////////////////////////////////////////////////////////////////////////////

		if (hovered_node && consider_hover)
		{
			element_input_ = NULL;
			element_output_ = NULL;

			for (int input_idx = 0; input_idx < hovered_node->inputs_.size(); ++input_idx)
			{
				if (hovered_node->inputs_[input_idx].state_ & NodesConnectorStateFlag_Hovered)
				{
					element_input_ = &hovered_node->inputs_[input_idx];
					state_ = NodesState_HoveringInput;
					break;
				}
			}						
			
			for (int output_idx = 0; output_idx < hovered_node->outputs_.size(); ++output_idx)
			{
				if (hovered_node->outputs_[output_idx].state_ & NodesConnectorStateFlag_Hovered)
				{
					element_output_ = &hovered_node->outputs_[output_idx];
					state_ = NodesState_HoveringOutput;
					break;
				}
			}					

			if (!element_input_ && !element_output_)
				state_ = NodesState_HoveringNode;	
		}

		////////////////////////////////////////////////////////////////////////////////

		if (state_ == NodesState_DragingInput)
		{
			element_output_ = NULL;
		
			if (hovered_node)
				for (int output_idx = 0; output_idx < hovered_node->outputs_.size(); ++output_idx)
				{
					NodesConnectorState state = hovered_node->outputs_[output_idx].state_;

					if (state & NodesConnectorStateFlag_Hovered && state & NodesConnectorStateFlag_Consider)
						element_output_ = &hovered_node->outputs_[output_idx];
				}
		}
		
		if (state_ == NodesState_DragingOutput)
		{
			element_input_ = NULL;
		
			if (hovered_node)
				for (int input_idx = 0; input_idx < hovered_node->inputs_.size(); ++input_idx)
				{
					NodesConnectorState state = hovered_node->inputs_[input_idx].state_;

					if (state & NodesConnectorStateFlag_Hovered && state & NodesConnectorStateFlag_Consider)
						element_input_ = &hovered_node->inputs_[input_idx];
				}
		}

		////////////////////////////////////////////////////////////////////////////////

		if (consider_hover)
		{
			element_node_ = hovered_node;

			if (!hovered_node)
				state_ = NodesState_Default;
		}					

		////////////////////////////////////////////////////////////////////////////////
	
		if (ImGui::IsMouseDoubleClicked(0))
		{
			switch (state_)
			{
				case NodesState_Default:
				{
					bool selected = false;

					for (int node_idx = 0; node_idx < nodes_.size(); ++node_idx)
					{
					    NodesState& state = nodes_[node_idx]->state_;

					    if (state & NodesNodeStateFlag_Selected)
						    selected = true;

						state &= ~(NodesNodeStateFlag_Selected | NodesNodeStateFlag_Marked | NodesNodeStateFlag_Hovered);
					}

					if (processing_node_ && false == selected)
					{
						processing_node_->state_ &= ~(NodesNodeStateFlag_Processing);
						processing_node_ = NULL;
					}				  

					return;
				};
	
				case NodesState_HoveringInput:
			    {
					if (element_input_->target_)
					{
						element_input_->output_->connections_--;
					    element_input_->output_ = NULL;
						element_input_->target_ = NULL;

						state_ = NodesState_DragingInput;
					}

					return;
				}

				case NodesState_HoveringNode:
				{
				    IM_ASSERT(element_node_);

					if (element_node_->inputs_.empty() && element_node_->outputs_.empty())
				    {
						state_ = NodesState_Draging;
						return;			    
					}

					if (element_node_->state_ & NodesNodeStateFlag_Collapsed)
					{					
						element_node_->state_ &= ~NodesNodeStateFlag_Collapsed;
						element_node_->area_node_.Max.y += element_node_->body_height_;

						const auto center = element_node_->area_node_.GetCenter().x;

						const auto width = element_node_->collapsed_width_ * 0.5f;
						element_node_->area_node_.Min.x = center - width;
						element_node_->area_node_.Max.x = center + width;

						element_node_->collapsed_width_ = ImGui::CalcTextSize(element_node_->name_).x;
					}
					else
					{
						element_node_->state_ |= NodesNodeStateFlag_Collapsed;
						element_node_->area_node_.Max.y -= element_node_->body_height_;
						
						const auto width = element_node_->area_node_.GetWidth();

						const auto center = element_node_->area_node_.GetCenter().x;
						element_node_->area_node_.Min.x = center - element_node_->collapsed_width_;
						element_node_->area_node_.Max.x = center + element_node_->collapsed_width_;

						element_node_->collapsed_width_ = width;
					}
	
					state_ = NodesState_Draging;
					return;
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////
		
		if (ImGui::IsMouseDoubleClicked(1))
		{
			switch (state_)
			{
				case NodesState_HoveringNode:
				{
					IM_ASSERT(hovered_node);

					if (hovered_node->state_ & NodesNodeStateFlag_Disabled)
						hovered_node->state_ &= ~(NodesNodeStateFlag_Disabled);
					else
						hovered_node->state_ |= (NodesNodeStateFlag_Disabled);

					return;
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		if (ImGui::IsMouseClicked(0))
		{
			switch (state_)
			{				
				case NodesState_HoveringNode:
				{
					if (io.KeyCtrl)
						element_node_->state_ ^= NodesNodeStateFlag_Selected;

					if (io.KeyShift)
						element_node_->state_ |= NodesNodeStateFlag_Selected;

					bool selected = element_node_->state_ & NodesNodeStateFlag_Selected;

					if (false == selected)
					{
						if (processing_node_)
							processing_node_->state_ &= ~(NodesNodeStateFlag_Processing);

						element_node_->state_ |= NodesNodeStateFlag_Processing;
						processing_node_ = element_node_;

						IM_ASSERT(false == nodes_.empty());

						if (nodes_.back() != element_node_)
					    {
							NodesNode** iterator = nodes_.find(element_node_);
						    nodes_.erase(iterator);
							nodes_.push_back(element_node_);
						}
					}
					else
						SortSelectedNodesOrder();
	
					state_ = NodesState_Draging;
					return;
				}
	
				case NodesState_HoveringInput:
				{
					if (!element_input_->target_)
						state_ = NodesState_DragingInput;
					else
						state_ = NodesState_Draging;

					return;
				}
	
				case NodesState_HoveringOutput:
				{
					state_ = NodesState_DragingOutput;
					return;
				}
			}

			return;
		}

		////////////////////////////////////////////////////////////////////////////////
	
		if (ImGui::IsMouseDragging(0))
		{
			switch (state_)
			{
				case NodesState_Default:
				{
					ImRect canvas(pos_, pos_ + size_);
					if (false == canvas.Contains(mouse_))
						return;
	
					if (false == io.KeyShift)
						for (int node_idx = 0; node_idx < nodes_.size(); ++node_idx)
							nodes_[node_idx]->state_ &= ~(NodesNodeStateFlag_Selected | NodesNodeStateFlag_Marked);
	
					state_ = NodesState_Selecting;
					return;
				}
	
				case NodesState_Selecting:
				{
					const ImVec2 pos = mouse_ - ImGui::GetMouseDragDelta(0);
	
					area_.Min = ImMin(pos, mouse_);
					area_.Max = ImMax(pos, mouse_);
	
					return;
				}
	
				case NodesState_Draging:
				{
					if (element_input_ && element_input_->output_ && element_input_->output_->connections_ > 0)
				        return;

					if (false == (element_node_->state_ & NodesNodeStateFlag_Selected))
						element_node_->TranslateNode(io.MouseDelta / scale_, false);
					else
						for (int node_idx = 0; node_idx < nodes_.size(); ++node_idx)
							nodes_[node_idx]->TranslateNode(io.MouseDelta / scale_, true);
	
					return;
				}
	
				case NodesState_DragingInput:
				{
					ImVec2 offset = pos_ + scroll_;
					ImVec2 p1 = offset + (element_input_->pos_ * scale_);
					ImVec2 p4 = element_output_ ? (offset + (element_output_->pos_ * scale_)) : mouse_;
	
					connection_ = ImVec4(p1.x, p1.y, p4.x, p4.y);
					return;
				}
	
				case NodesState_DragingOutput:
				{
					ImVec2 offset = pos_ + scroll_;
					ImVec2 p1 = offset + (element_output_->pos_ * scale_);
					ImVec2 p4 = element_input_ ? (offset + (element_input_->pos_ * scale_)) : mouse_;

					connection_ = ImVec4(p4.x, p4.y, p1.x, p1.y);
					return;
				}
			}
	
			return;
		}
	
		////////////////////////////////////////////////////////////////////////////////

		if (ImGui::IsMouseReleased(0))
		{
			switch (state_)
			{
			case NodesState_Selecting:
			{
				element_node_ = NULL;
				element_input_ = NULL;
				element_output_ = NULL;

				area_ = {};

				////////////////////////////////////////////////////////////////////////////////

				SortSelectedNodesOrder();
				state_ = NodesState_Default;
				return;
			}

			case NodesState_Draging:
			{
				state_ = NodesState_HoveringNode;
				return;
			}

			case NodesState_DragingInput:
			case NodesState_DragingOutput:
			{
				if (element_input_ && element_output_)
				{
					IM_ASSERT(hovered_node);
					IM_ASSERT(element_node_);
					element_input_->target_ = state_ == NodesState_DragingInput ? hovered_node : element_node_;

					if (element_input_->output_)
						element_input_->output_->connections_--;

					element_input_->output_ = element_output_;
					element_output_->connections_++;
				}

				connection_ = ImVec4();
				state_ = NodesState_Default;
				return;
			}
			}

			return;
		}

		////////////////////////////////////////////////////////////////////////////////
		
		if (ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_Delete]))
		{
			ImVector<NodesNode*> nodes;
			nodes.reserve(nodes_.size());

			for (int node_idx = 0; node_idx < nodes_.size(); ++node_idx)
			{
				NodesNode* node = nodes_[node_idx];
				IM_ASSERT(node);

				if (node->state_ & NodesNodeStateFlag_Selected)
				{
					element_node_ = NULL;
					element_input_ = NULL;
					element_output_ = NULL;

					state_ = NodesState_Default;

					for (int sweep_idx = 0; sweep_idx < nodes_.size(); ++sweep_idx)
					{
						NodesNode* sweep = nodes_[sweep_idx];
						IM_ASSERT(sweep);
					
						for (int input_idx = 0; input_idx < sweep->inputs_.size(); ++input_idx)
						{
							NodesInput& input = sweep->inputs_[input_idx];

							if (node == input.target_)
							{
								if (input.output_)
									input.output_->connections_--;

								input.target_ = NULL;
								input.output_ = NULL;
							}
						}
					}

					for (int input_idx = 0; input_idx < node->inputs_.size(); ++input_idx)
					{
						NodesInput& input = node->inputs_[input_idx];
						
						if (input.output_)
							input.output_->connections_--;
						
						input.type_ = NULL;
						input.name_ = NULL;
						input.target_ = NULL;
						input.output_ = NULL;
					}

					for (int output_idx = 0; output_idx < node->outputs_.size(); ++output_idx)
					{
						NodesOutput& output = node->outputs_[output_idx];
						IM_ASSERT(output.connections_ == 0);
					}

					if (node == processing_node_)
						processing_node_ = NULL;
		
					delete node;
				}
				else
				{
					nodes.push_back(node);
				}			
			}

			nodes_ = nodes;

			return;
		}
	}

	void Nodes::ProcessNodes()
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		const ImVec2 offset = pos_ + scroll_;

		////////////////////////////////////////////////////////////////////////////////

		ImGui::SetWindowFontScale(scale_);

		for (int node_idx = 0; node_idx < nodes_.size(); ++node_idx)
		{
			const NodesNode* node = nodes_[node_idx];
			IM_ASSERT(node);

			for (int input_idx = 0; input_idx < node->inputs_.size(); ++input_idx)
			{		
				const NodesInput& input = node->inputs_[input_idx];
				
				if (const NodesNode* target = input.target_)
				{
					IM_ASSERT(target);

					ImVec2 p1 = offset;
					ImVec2 p4 = offset;

					if (node->state_ & NodesNodeStateFlag_Collapsed)
					{
						ImVec2 collapsed_input = { 0, (node->area_node_.Max.y - node->area_node_.Min.y) * 0.5f };					

						p1 += ((node->area_node_.Min + collapsed_input) * scale_);
					}
					else
					{
						p1 += (input.pos_ * scale_);
					}

					if (target->state_ & NodesNodeStateFlag_Collapsed)
					{
						ImVec2 collapsed_output = { 0, (target->area_node_.Max.y - target->area_node_.Min.y) * 0.5f };					
						
						p4 += ((target->area_node_.Max - collapsed_output) * scale_);
					}
					else
					{
						p4 += (input.output_->pos_ * scale_);		
					}

					DrawConnection(p1, p4, ImColor(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
		}

		for (int node_idx = 0; node_idx < nodes_.size(); ++node_idx)
		{
			const NodesNode* node = nodes_[node_idx];
			IM_ASSERT(node);
			node->DrawNode(draw_list, offset, scale_, state_);
		}

		if (connection_.x != connection_.z && connection_.y != connection_.w)
			DrawConnection(ImVec2(connection_.x, connection_.y), ImVec2(connection_.z, connection_.w), ImColor(0.0f, 1.0f, 0.0f, 1.0f));

		ImGui::SetWindowFontScale(1.0f);

		////////////////////////////////////////////////////////////////////////////////

		if (state_ == NodesState_Selecting)
		{
			draw_list->AddRectFilled(area_.Min, area_.Max, ImColor(1.0f, 1.0f, 0.0f, 0.1f));
			draw_list->AddRect(area_.Min, area_.Max, ImColor(1.0f, 1.0f, 0.0f, 0.5f));
		}

		////////////////////////////////////////////////////////////////////////////////

		ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
		ImGui::NewLine();
		
		switch (state_)
		{
			case NodesState_Default: ImGui::Text("NodesState_Default"); break;
			case NodesState_HoveringNode: ImGui::Text("NodesState_HoveringNode"); break;
			case NodesState_HoveringInput: ImGui::Text("NodesState_HoveringInput"); break;
			case NodesState_HoveringOutput: ImGui::Text("NodesState_HoveringOutput"); break;
			case NodesState_Draging: ImGui::Text("NodesState_Draging"); break;
			case NodesState_DragingInput: ImGui::Text("NodesState_DragingInput"); break;
			case NodesState_DragingOutput: ImGui::Text("NodesState_DragingOutput"); break;
			case NodesState_Selecting: ImGui::Text("NodesState_Selecting"); break;
			default: ImGui::Text("UNKNOWN"); break;
		}

		ImGui::NewLine();

		ImGui::Text("Position: %.2f, %.2f", pos_.x, pos_.y);
		ImGui::Text("Size: %.2f, %.2f", size_.x, size_.y);
		ImGui::Text("Mouse: %.2f, %.2f", mouse_.x, mouse_.y);
		ImGui::Text("Scroll: %.2f, %.2f", scroll_.x, scroll_.y);
		ImGui::Text("Scale: %.2f", scale_);
	
		ImGui::NewLine();
		
		if (element_node_)
			ImGui::Text("Element_node: %s", element_node_->name_);
		if (element_input_)
			ImGui::Text("Element_input: %s", element_input_->name_);
		if (element_output_)
			ImGui::Text("Element_output: %s", element_output_->name_);


		////////////////////////////////////////////////////////////////////////////////
	}

	void Nodes::ProcessContextMenu()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

		if (ImGui::BeginPopup("NodesContextMenu"))
		{
			for (int node_idx = 0; node_idx < nodes_desc_.size(); ++node_idx)
				if (ImGui::MenuItem(nodes_desc_[node_idx].name_))
				{
					ImVec2 position = (mouse_ - scroll_ - pos_) / scale_;
					NodesNode* node = CreateNodeFromDesc(&nodes_desc_[node_idx], position);
					nodes_.push_back(node);
				}

			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
	}
}