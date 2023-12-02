#include "_editor.hpp"

namespace madX::Editor::Panel
{
	void NodesEditor::UpdateCanvasGeometry()
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

		if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Home))
		{
			scroll_ = {};
			scale_ = 1.0f;
		}
				
		////////////////////////////////////////////////////////////////////////////////

		if (ImGui::IsWindowFocused() && false == ImGui::IsMouseDown(0) && canvas.Contains(mouse_))
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
						scale_ = ImMax(0.2f, scale_ / 1.05f);

				if (io.MouseWheel > 0.0f)
					for (float zoom = io.MouseWheel; zoom > 0.0f; zoom -= 1.0f)
						scale_ = ImMin(10.0f, scale_ * 1.05f);

				ImVec2 shift = scroll_ + (focus * scale_);
				scroll_ += mouse_ - shift - pos_;
			}

			if (ImGui::IsMouseReleased(1) && element_hovered_ == nullptr)
			{
				if (io.MouseDragMaxDistanceSqr[1] < (io.MouseDragThreshold * io.MouseDragThreshold))
				{
					bool selected = false;

					for (const auto& node : nodes_)
						if (node->state_ & Meta::Nodes::NodeStateFlag_Selected)
						{
							selected = true;
							break;
						}

					if (false == selected)
						ImGui::OpenPopup("NodesContextMenu");
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		const float grid = Meta::Nodes::NodesSnap * scale_ * 4.0f;

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		
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

	////////////////////////////////////////////////////////////////////////////////

	void NodesEditor::UpdateNodesOnCanvas()
	{
		element_input_ = nullptr;
		element_hovered_ = nullptr;

		if (nodes_.empty())
			return;

		const ImGuiIO& io = ImGui::GetIO();

		const ImVec2 offset = pos_ + scroll_;
		const ImRect canvas(pos_, pos_ + size_);

		for (auto node_idx = nodes_.size(); node_idx != 0;)
		{
			Meta::Nodes::Node* node = nodes_[--node_idx].get();
			IM_ASSERT(node);

			constexpr Meta::Nodes::NodeState mask = Meta::Nodes::NodeStateFlag_Selected | Meta::Nodes::NodeStateFlag_Processing | Meta::Nodes::NodeStateFlag_Rooted;
			node->state_ &= mask;

			ImRect node_rect = Meta::Nodes::GetImRectOnConvas(node->pos_, node->area_node_, offset, scale_);

			node_rect.ClipWith(canvas);

			if (canvas.Overlaps(node_rect))
				node->state_ |= Meta::Nodes::NodeStateFlag_Visible;
			else
				continue;

			////////////////////////////////////////////////////////////////////////////////

			if (state_ == NodesState_Selecting)
			{
				if (io.KeyCtrl && area_.Overlaps(node_rect))
				{
					node->state_ |= Meta::Nodes::NodeStateFlag_Marked;
					continue;
				}

				if (false == io.KeyCtrl && area_.Contains(node_rect))
				{
					node->state_ |= Meta::Nodes::NodeStateFlag_Marked;
					continue;
				}

				node->state_ &= ~Meta::Nodes::NodeStateFlag_Marked;
			}

			////////////////////////////////////////////////////////////////////////////////

			if (element_hovered_)
				continue;

			if (node_rect.Contains(mouse_))
			{
				element_hovered_ = node;
			}

			if (state_ == NodesState_DragingOutput)
			{
				IM_ASSERT(element_output_);

				if (element_output_ == node)
					continue;

				const ImRect parent_rect = Meta::Nodes::GetImRectOnConvas(node->pos_, node->area_parent_, offset, scale_);

				if (parent_rect.Contains(mouse_))
				{
					element_hovered_ = node;
					continue;
				}

				if (node->inputs_.empty())
					continue;

				if (element_output_->type_ & Meta::Nodes::NodeType_Root)
					continue;

				const float rounding = Meta::Nodes::NodesRoundingRadius * scale_;
				ImVec2 input_pos = Meta::Nodes::GetInputPos(node_rect, rounding);

				for (auto& input : node->inputs_)
				{
					const ImRect area_input = Meta::Nodes::GetInputArea(input_pos, rounding);

					if (area_input.Contains(mouse_))
					{
						input.state_ |= Meta::Nodes::InputStateFlag_Hovered;

						element_hovered_ = node;
						element_input_ = &input;
					}
					else
					{
						input.state_ &= ~Meta::Nodes::InputStateFlag_Hovered;
					}

					input_pos += Meta::Nodes::GetInputSeparation(rounding);
				}	
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	void NodesEditor::SplitOrderOnSelectedNodes()
	{
		//TODO: optimize me!!!
		Meta::Nodes::NodesContainer nodes_unselected;
		nodes_unselected.reserve(nodes_.size());
		
		Meta::Nodes::NodesContainer nodes_selected;
		nodes_selected.reserve(nodes_.size());
		
		for (auto index = 0; index < nodes_.size(); ++index)
		{
			auto node = nodes_[index].get();
		
			if (node->state_ & Meta::Nodes::NodeStateFlag_Marked || node->state_ & Meta::Nodes::NodeStateFlag_Selected)
			{
				node->state_ &= ~Meta::Nodes::NodeStateFlag_Marked;
				node->state_ |= Meta::Nodes::NodeStateFlag_Selected;
				nodes_selected.emplace_back(std::move(nodes_[index]));
			}
			else
				nodes_unselected.emplace_back(std::move(nodes_[index]));
		}


		std::size_t node_idx = 0;
		
		for (auto& unselected : nodes_unselected)
		{
			nodes_[node_idx++] = std::move(unselected);
		}
		
		for (auto& selected : nodes_selected)
		{
			nodes_[node_idx++] = std::move(selected);
		}	
	}

	void NodesEditor::SweepRootPaths(Meta::Nodes::Node* root)
	{
		IM_ASSERT(root);
	
		root->state_ |= Meta::Nodes::NodeStateFlag_Dirty;
		
		if (root->state_ & Meta::Nodes::NodeStateFlag_Rooted)
		{
			for (auto path : root->paths_)
			{
				if (path->state_ & Meta::Nodes::NodeStateFlag_Dirty)
					continue;
	
				path->state_ |= Meta::Nodes::NodeStateFlag_Rooted;
				SweepRootPaths(path);
			}
		}
		else
		{
			for (auto path : root->paths_)
			{	
				if (path->state_ & Meta::Nodes::NodeStateFlag_Dirty)
					continue;
			
				path->state_ &= ~Meta::Nodes::NodeStateFlag_Rooted;
				SweepRootPaths(path);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	
	Meta::UniquePtr<Meta::Nodes::Node> NodesEditor::CreateNode(const Meta::String& name, const Meta::Nodes::NodeDesc& desc, ImVec2 pos)
	{
		//TODO: move BuildNode to Meta::Nodes::Node ctor
		auto node = Meta::MakeUniquePtr<Meta::Nodes::Node>();
		node->BuildNode(name, desc.type_, Meta::Nodes::GetImColorFromNodeType(desc.type_));

		node->properties_.emplace_back();

		for (const auto& input : desc.inputs_)
			node->inputs_.push_back({ input.name_ });

		////////////////////////////////////////////////////////////////////////////////

		node->TranslateNode(pos - node->area_node_.GetCenter());
		node->state_ |= Meta::Nodes::NodeStateFlag_Visible | Meta::Nodes::NodeStateFlag_Processing;

		////////////////////////////////////////////////////////////////////////////////

		if (processing_node_)
		{
			processing_node_->state_ &= ~Meta::Nodes::NodeStateFlag_Processing;
		}

		panel_state_.processing_node_ = processing_node_ = node.get();
		MarkModified();

		return node;
	}	

	////////////////////////////////////////////////////////////////////////////////

	void NodesEditor::Update()
	{
		UpdateCanvasGeometry();

		if (false == ImGui::IsWindowFocused())
		{
			return;
		}

		////////////////////////////////////////////////////////////////////////////////

		const ImVec2 offset = pos_ + scroll_;
		const ImGuiIO& io = ImGui::GetIO();
		
		UpdateNodesOnCanvas();

		{
			bool consider_hover = false;
			consider_hover |= state_ == NodesState_Default;
			consider_hover |= state_ == NodesState_HoveringNode;
			consider_hover |= state_ == NodesState_HoveringOutput;

			if (consider_hover)
			{
				element_dragging_ = element_hovered_;

				if (element_hovered_)
				{
					const ImRect out_rect = Meta::Nodes::GetImRectOnConvas(element_hovered_->pos_, element_hovered_->area_out_, offset, scale_);
				
					if (out_rect.Contains(mouse_))
					{
						state_ = NodesState_HoveringOutput;
						element_output_ = element_hovered_;
					}
					else
					{
						state_ = NodesState_HoveringNode;
						element_output_ = nullptr;
					}			
				}
				else
				{
					state_ = NodesState_Default;
				}
			}
			else if (state_ == NodesState_DragingOutput && element_hovered_)
			{
				if (element_hovered_ != element_output_ && nullptr == element_input_)
				{
					element_hovered_->state_ |= Meta::Nodes::NodeStateFlag_TabTop;		
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		if (element_output_)
		{
			if (state_ == NodesState_HoveringOutput || state_ == NodesState_DragingOutput)
				element_output_->state_ |= Meta::Nodes::NodeStateFlag_TabRight;		
			else
				element_output_ = nullptr;
		}

		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
	
		if (ImGui::IsMouseDoubleClicked(0))
		{
			switch (state_)
			{
			case NodesState_Default:
			{
				bool selected = false;

				for (auto& node : nodes_)
				{
				    if (node->state_ & Meta::Nodes::NodeStateFlag_Selected)
					    selected = true;

					node->state_ &= ~(Meta::Nodes::NodeStateFlag_Selected | Meta::Nodes::NodeStateFlag_Marked);
				}

				if (processing_node_ && false == selected)
				{
					processing_node_->state_ &= ~Meta::Nodes::NodeStateFlag_Processing;
					panel_state_.processing_node_ = processing_node_ = nullptr;
				}				  

				return;
			};
			case NodesState_HoveringNode:
			{
				if (element_hovered_ && element_hovered_->type_ & Meta::Nodes::NodeType_Root)
				{
					IM_ASSERT(nullptr == element_output_);

					element_output_ = element_hovered_;
					state_ = NodesState_DragingOutput;
				}
				
				return;
			}
			default:
				return;
			}
		}

		////////////////////////////////////////////////////////////////////////////////
		
		if (ImGui::IsMouseDoubleClicked(1))
		{
			switch (state_)
			{
			case NodesState_HoveringNode:
			{
				IM_ASSERT(element_hovered_);
				IM_ASSERT(element_dragging_);
				IM_ASSERT(element_hovered_ == element_dragging_);

				if (false == (element_hovered_->state_ & Meta::Nodes::NodeStateFlag_Selected))
				{
					element_hovered_->type_ ^= Meta::Nodes::NodeFlag_Disabled;
				}
				else
				{
					if (element_hovered_->type_ & Meta::Nodes::NodeFlag_Disabled)
					{
						for (auto& node : nodes_)
						{
							if (false == (node->state_ & Meta::Nodes::NodeStateFlag_Selected))
								continue;

							node->type_ &= ~Meta::Nodes::NodeFlag_Disabled;
						}
					}
					else
					{
						for (auto& node : nodes_)
						{
							if (false == (node->state_ & Meta::Nodes::NodeStateFlag_Selected))
								continue;

							node->type_ |= Meta::Nodes::NodeFlag_Disabled;							
						}
					}
				}

				return;
			}
		
			default:
				return;
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		if (ImGui::IsMouseClicked(0))
		{
			switch (state_)
			{				
			case NodesState_Default:
			{
				if ((io.KeyShift && io.KeyCtrl) && connection_node_)
				{
					if (connection_path_)
					{
						const auto found = std::find(connection_node_->paths_.begin(), connection_node_->paths_.end(), connection_path_);
						
						if (found != connection_node_->paths_.end())
						{
							IM_ASSERT(*found == connection_path_);
							AssertDirtyNodes();

							connection_path_->state_ &= ~Meta::Nodes::NodeStateFlag_Rooted;
							SweepRootPaths(connection_path_);
							ClearDirtyFlag();

							connection_node_->paths_.erase(found);

							PropagateRootedFlag();
							ClearDirtyFlag();

							MarkModified();

							connection_node_->type_ |= Meta::Nodes::NodeFlag_Modified;
							connection_path_->type_ |= Meta::Nodes::NodeFlag_Modified;
						}
						else
						{
							IM_ASSERT(false);
						}
					}
					else if (connection_input_)
					{
						IM_ASSERT(connection_input_->source_);
						
						connection_input_->source_->type_ |= Meta::Nodes::NodeFlag_Modified;
						connection_node_->type_ |= Meta::Nodes::NodeFlag_Modified;

						connection_input_->source_ = nullptr;

						MarkModified();
					}
					else
					{
						IM_ASSERT(false);
					}
					
					return;
				}

				////////////////////////////////////////////////////////////////////////////////

				area_ = {};
				
				const ImRect canvas(pos_, pos_ + size_);
				if (false == canvas.Contains(mouse_))
					return;
				
				if (false == io.KeyShift)
					for (auto& node : nodes_)
						node->state_ &= ~(Meta::Nodes::NodeStateFlag_Selected | Meta::Nodes::NodeStateFlag_Marked);
				
				state_ = NodesState_Selecting;

				return;
			}
			case NodesState_HoveringNode:
			{
				IM_ASSERT(element_hovered_);

				const bool selected = element_hovered_->state_ & Meta::Nodes::NodeStateFlag_Selected;

				if (io.KeyShift)
					element_hovered_->state_ |= Meta::Nodes::NodeStateFlag_Selected;

				if (io.KeyCtrl)
					element_hovered_->state_ ^= Meta::Nodes::NodeStateFlag_Selected;

				if (false == selected && (false == (element_hovered_->state_ & Meta::Nodes::NodeStateFlag_Selected)))
				{
					if (processing_node_)
						processing_node_->state_ &= ~(Meta::Nodes::NodeStateFlag_Processing);
				
					element_hovered_->state_ |= Meta::Nodes::NodeStateFlag_Processing;
					panel_state_.processing_node_ = processing_node_ = element_hovered_;
				
					IM_ASSERT(false == nodes_.empty());
				
					if (nodes_.back().get() != element_hovered_)
					{
						for (auto node = nodes_.begin(); node != nodes_.end(); ++node)
							if (node->get() == element_hovered_)
								std::swap(*node, *(nodes_.end() - 1));
					}
				}
				else
				{
					SplitOrderOnSelectedNodes();
				}
	
				state_ = NodesState_Draging;

				for (auto& node : nodes_)
					node->pos_ = Meta::Nodes::GetSnapPosition(node->pos_);

				return;
			}
			case NodesState_HoveringOutput:
			{
				IM_ASSERT(element_hovered_);
				IM_ASSERT(element_output_);
				
				state_ = NodesState_DragingOutput;
				return;
			}
			default:
				return;
			}
		}

		////////////////////////////////////////////////////////////////////////////////
	
		if (ImGui::IsMouseDragging(0))
		{
			switch (state_)
			{
			case NodesState_Selecting:
			{
				const ImVec2 pos = mouse_ - ImGui::GetMouseDragDelta(0);
	
				area_.Min = ImMin(pos, mouse_);
				area_.Max = ImMax(pos, mouse_);
	
				return;
			}
			case NodesState_Draging:
			{
				IM_ASSERT(element_dragging_);

				if (false == (element_dragging_->state_ & Meta::Nodes::NodeStateFlag_Selected))
					element_dragging_->TranslateNode(io.MouseDelta / scale_, false);
				else
					for (auto& node : nodes_)
						node->TranslateNode(io.MouseDelta / scale_, true);
			
				return;
			}
			case NodesState_DragingOutput:
			{
				IM_ASSERT(element_output_);

				ImVec2 p1;

				if (false == (element_output_->type_ & Meta::Nodes::NodeType_Root))
				{
					const ImRect out = Meta::Nodes::GetImRectOnConvas(element_output_->pos_, element_output_->area_out_, offset, scale_);
					p1 = ImVec2(out.Max.x, out.GetCenter().y);
				}
				else
				{
					const ImRect body = Meta::Nodes::GetImRectOnConvas(element_output_->pos_, element_output_->area_body_, offset, scale_);
					p1 = body.GetCenter();
				}

				connection_ = ImRect(p1, mouse_);

				return;
			}
			default:
				return;
			}
		}
	
		////////////////////////////////////////////////////////////////////////////////

		if (ImGui::IsMouseReleased(0))
		{
			switch (state_)
			{
			case NodesState_Selecting:
			{
				area_ = {};

				SplitOrderOnSelectedNodes();
				state_ = NodesState_Default;

				return;
			}

			case NodesState_Draging:
			{
				state_ = NodesState_HoveringNode;

				IM_ASSERT(element_dragging_);

				if (element_dragging_->type_ & Meta::Nodes::NodeFlag_Modified)
				{
					AssertDirtyNodes();
					MarkModified();
				}

				return;
			}

			case NodesState_DragingOutput:
			{				
				state_ = NodesState_Default;

				if (element_hovered_ && (element_output_ != element_hovered_))
				{
					if (element_input_)
					{
						if (element_input_->source_ != element_output_)
							MarkModified();

						element_input_->source_ = element_output_;
						element_input_->state_ = Meta::Nodes::InputStateFlag_Default;
					}
					else if (element_hovered_->type_ & Meta::Nodes::NodeFlag_Inherit)
					{				
						//TODO: only root-like as a source
						
						const auto found = std::find(element_output_->paths_.begin(), element_output_->paths_.end(), element_hovered_);
						if (found != element_output_->paths_.end())
						{
							IM_ASSERT(*found == element_hovered_);
							return;
						}

						element_output_->paths_.push_back(element_hovered_);

						element_output_->type_ |= Meta::Nodes::NodeFlag_Modified;
						element_hovered_->type_ |= Meta::Nodes::NodeFlag_Modified;

						////////////////////////////////////////////////////////////////////////////////
						
						//TODO: propagate Meta::Nodes::NodeFlag_Modified

						AssertDirtyNodes();

						if (element_output_->state_ & Meta::Nodes::NodeStateFlag_Rooted)
						{
							element_hovered_->state_ |= Meta::Nodes::NodeStateFlag_Rooted;
							SweepRootPaths(element_hovered_);
						}

						ClearDirtyFlag();
						MarkModified();
					}
				}

				return;
			}
			default:
				return;
			}
		}

		////////////////////////////////////////////////////////////////////////////////
		
		if (ImGui::IsKeyPressed(ImGuiKey_Delete))
		{			
			//TODO: bug with more than one ROOT and one deleted - no propagation

			AssertDirtyNodes();

			for (auto& node : nodes_)
			{
				for (auto& input : node->inputs_)
				{
					if (input.source_ && (input.source_->state_ & Meta::Nodes::NodeStateFlag_Selected))
					{
						input.source_ = nullptr;
					}
				}

				for (auto path = node->paths_.begin(); path != node->paths_.end(); path)
				{
					if ((*path)->state_& Meta::Nodes::NodeStateFlag_Selected)
					{
						(*path)->state_ &= ~Meta::Nodes::NodeStateFlag_Rooted;
						SweepRootPaths(*path);

						path = node->paths_.erase(path);
					}
					else
						++path;
				}
			}

			ClearDirtyFlag();
			bool modified = false;

			for (auto node = nodes_.begin(); node != nodes_.end(); node)
			{
				if (node->get()->state_ & Meta::Nodes::NodeStateFlag_Selected)
				{
					if (node->get() == processing_node_)
						panel_state_.processing_node_ = processing_node_ = nullptr;

					node = nodes_.erase(node);
					modified = true;
				}
				else
					++node;
			}	

			PropagateRootedFlag();
			ClearDirtyFlag();

			if (modified)
				MarkModified();

			return;
		}

		////////////////////////////////////////////////////////////////////////////////

		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false))
		{	
			cista::file file{ "test.bin", "w+" };
			cista::serialize<Meta::Detail::MODE>(file, nodes_);
			
			return;
		}

		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_L, false))
		{
			stream_ = Core::Filesystem::Stream("test.bin");

			auto deserialized = cista::deserialize<Meta::Nodes::NodesContainer, Meta::Detail::MODE>(stream_.GetStream());
			std::swap(nodes_, *deserialized);

			state_ = NodesState_Default;

			element_dragging_ = nullptr;
			element_output_ = nullptr;
			element_hovered_ = nullptr;
			element_input_ = nullptr;

			connection_node_ = nullptr;
			connection_path_ = nullptr;
			connection_input_ = nullptr;

			area_ = {};
			connection_ = {};

			panel_state_.processing_node_ = processing_node_ = nullptr;

			for (const auto& node : nodes_)
				if (node->state_ & Meta::Nodes::NodeStateFlag_Processing)
				{
					panel_state_.processing_node_ = processing_node_ = node.get();
					break;
				}
		}

		////////////////////////////////////////////////////////////////////////////////

		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z, false))
		{
		}

		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y, false))
		{
		}

		////////////////////////////////////////////////////////////////////////////////

		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Space, false))
		{
			UnMarkModified();

			for (auto& node : nodes_)
				node->type_ &= ~Meta::Nodes::NodeFlag_Modified;
		}

		return;
	}

	////////////////////////////////////////////////////////////////////////////////

	void NodesEditor::ProcessNodes()
	{
		const ImGuiIO& io = ImGui::GetIO();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		const ImVec2 offset = pos_ + scroll_;

		////////////////////////////////////////////////////////////////////////////////

		ImGui::SetWindowFontScale(scale_);

		connection_node_ = nullptr;
		connection_path_ = nullptr;
		connection_input_ = nullptr;

		if (state_ != NodesState_DragingOutput)
		{
			connection_ = {};
		}

		//TODO: break it
		for (auto& node : nodes_)
		{
			if (false == node->paths_.empty())
			{
				const ImColor color = ImColor(0.75f, 0.75f, 0.75f, 1.0f);

				ImVec2 p1;

				if (false == (node->type_ & Meta::Nodes::NodeType_Root))
				{
					const ImRect out = Meta::Nodes::GetImRectOnConvas(node->pos_, node->area_out_, offset, scale_);
					p1 = ImVec2(out.Max.x, out.GetCenter().y);
				}
				else
				{
					const ImRect body = Meta::Nodes::GetImRectOnConvas(node->pos_, node->area_body_, offset, scale_);
					p1 = body.GetCenter();
				}
				
				for (const auto& path : node->paths_)
				{
					const ImRect parent = Meta::Nodes::GetImRectOnConvas(path->pos_, path->area_parent_, offset, scale_);
					const ImVec2 p4 = ImVec2(parent.GetCenter().x, parent.Min.y);

					if (state_ == NodesState_Default && (connection_node_ == nullptr))
					{
						const float dist = GetDistanceToConnection
						(
							p1, p4, mouse_,
							node->type_ & Meta::Nodes::NodeType_Root ? Meta::Nodes::ConnectionStyle_None : Meta::Nodes::ConnectionStyle_Right,
							Meta::Nodes::ConnectionStyle_Bottom
						);

						if (dist < Meta::Nodes::NodesDistanceToConnection * scale_)
						{
							connection_node_ = node.get();
							connection_path_ = path;
							connection_ = ImRect(p1, p4);
						}
					}

					DrawConnection
					(
						p1, p4, color,
						node->type_ & Meta::Nodes::NodeType_Root ? Meta::Nodes::ConnectionStyle_None : Meta::Nodes::ConnectionStyle_Right,
						Meta::Nodes::ConnectionStyle_Bottom
					);
				}
			}

			////////////////////////////////////////////////////////////////////////////////
			
			if (false == node->inputs_.empty())
			{
				const ImColor color = ImColor(0.25f, 0.75f, 0.75f, 1.0f);

				ImRect node_rect = Meta::Nodes::GetImRectOnConvas(node->pos_, node->area_node_, offset, scale_);
				const float rounding = Meta::Nodes::NodesRoundingRadius * scale_;
				ImVec2 input_pos = Meta::Nodes::GetInputPos(node_rect, rounding);

				for (auto& input : node->inputs_)
				{
					if (input.source_)
					{
						IM_ASSERT(false == (input.source_->type_ & Meta::Nodes::NodeType_Root));

						const ImRect area_input = Meta::Nodes::GetInputArea(input_pos, rounding);
						const ImRect area_out = Meta::Nodes::GetImRectOnConvas(input.source_->pos_, input.source_->area_out_, offset, scale_);

						const ImVec2 input_p1 = ImVec2(area_out.Max.x, area_out.GetCenter().y);
						const ImVec2 input_p4 = ImVec2(area_input.GetCenter().x, area_input.Max.y);

						if (state_ == NodesState_Default && (connection_node_ == nullptr))
						{
							const float dist = GetDistanceToConnection
							(
								input_p1, input_p4, mouse_,
								Meta::Nodes::ConnectionStyle_Right,
								Meta::Nodes::ConnectionStyle_Top
							);

							if (dist < Meta::Nodes::NodesDistanceToConnection * scale_)
							{
								connection_node_ = node.get();
								connection_input_ = &input;
								connection_ = ImRect(input_p1, input_p4);
							}
						}

						DrawConnection
						(
							input_p1, input_p4, color,
							Meta::Nodes::ConnectionStyle_Right,
							Meta::Nodes::ConnectionStyle_Top
						);		
					}

					input_pos += Meta::Nodes::GetInputSeparation(rounding);
				}
			}

			////////////////////////////////////////////////////////////////////////////////
		}

		if (connection_node_)
		{
			const ImColor color  = (io.KeyShift && io.KeyCtrl) ? ImColor(1.0f, 0.0f, 0.0f, 1.0f) : ImColor(1.0f, 1.0f, 0.0f, 1.0f);

			if (connection_path_)
			{
				DrawConnection
				(
					connection_.GetTL(), connection_.GetBR(), color,
					connection_node_->type_ & Meta::Nodes::NodeType_Root ? Meta::Nodes::ConnectionStyle_None : Meta::Nodes::ConnectionStyle_Right,
					Meta::Nodes::ConnectionStyle_Bottom
				);
			}
			else if (connection_input_)
			{
				DrawConnection
				(	
					connection_.GetTL(), connection_.GetBR(), color,
					Meta::Nodes::ConnectionStyle_Right,
					Meta::Nodes::ConnectionStyle_Top
				);			
			}
			else
			{
				IM_ASSERT(false);
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		for (const auto& node : nodes_)
		{
			if (node.get() == element_output_ && (node->type_ & Meta::Nodes::NodeType_Root))
				continue;

			node->Draw(draw_list, offset, scale_);
		}

		if (element_output_)
		{
			bool green = element_input_;
			green |= (element_hovered_ && (element_hovered_ != element_output_));
			
			const ImColor color = green ? ImColor(0.0f, 1.0f, 0.0f, 1.0f) : ImColor(1.0f, 1.0f, 1.0f, 1.0f);

			DrawConnection
			(
				connection_.GetTL(), connection_.GetBR(),
				color ,
				element_output_->type_ & Meta::Nodes::NodeType_Root ? Meta::Nodes::ConnectionStyle_None : Meta::Nodes::ConnectionStyle_Right,
				Meta::Nodes::ConnectionStyle_None
			);
		}

		if (element_output_ && (element_output_->type_ & Meta::Nodes::NodeType_Root))
		{
			element_output_->Draw(draw_list, offset, scale_);
		}

		////////////////////////////////////////////////////////////////////////////////

		ImGui::SetWindowFontScale(1.0f);

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
			case NodesState_HoveringOutput: ImGui::Text("NodesState_HoveringOutput"); break;
			case NodesState_Draging: ImGui::Text("NodesState_Draging"); break;
			case NodesState_DragingOutput: ImGui::Text("NodesState_DragingOutput"); break;
			case NodesState_Selecting: ImGui::Text("NodesState_Selecting"); break;
			default:
			{
				IM_ASSERT(false);
				ImGui::Text("UNKNOWN");
				break;
			}
		}

		ImGui::NewLine();

		ImGui::Text("Position: %.2f, %.2f", pos_.x, pos_.y);
		ImGui::Text("Size: %.2f, %.2f", size_.x, size_.y);
		ImGui::Text("Mouse: %.2f, %.2f", mouse_.x, mouse_.y);
		ImGui::Text("Scroll: %.2f, %.2f", scroll_.x, scroll_.y);
		ImGui::Text("Scale: %.2f", scale_);
//		ImGui::Text("MouseDistance[0]: %.2f, %.2f", mouse_.x - io.MouseClickedPos[0].x, mouse_.y - io.MouseClickedPos[0].y);
	
		//ImGui::Text(modified_ ? "MODIFIED" : "");
		ImGui::Text("Modified: %u :: %u", (modified_ & 0xffffffff), (modified_ >> 32u));

		ImGui::NewLine();
		
		if (element_dragging_)
			ImGui::Text("Element_dragging: %s", element_dragging_->name_.data());
		if (element_output_)
			ImGui::Text("Element_output: %s", element_output_->name_.data());
		if (processing_node_)
			ImGui::Text("Processing_node_: %s", processing_node_->name_.data());
		if (element_input_)
			ImGui::Text("Element_input: %s", element_input_->name_.data());
		if (element_hovered_)
			ImGui::Text("Element_hovered: %s", element_hovered_->name_.data());

		if (element_hovered_)
		{
			ImGui::NewLine();
			ImGui::Text("--- Element details ---");
			ImGui::Text("Position: %.2f, %.2f", element_hovered_->pos_.x, element_hovered_->pos_.y);
			ImGui::Text("State: %s", element_hovered_->type_ & Meta::Nodes::NodeFlag_Modified ? "Modified" : "None");
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	void NodesEditor::ProcessContextMenu()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

		if (ImGui::BeginPopup("NodesContextMenu"))
		{
			const float size = ImGui::GetTextLineHeight();

			for (const auto& iterator : Meta::Nodes::sNodesDescMap)
			{
				IM_ASSERT(iterator.second.type_ & Meta::Nodes::cNodeABI_TypeMask);
			
				ImVec2 pos = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size, pos.y + size), Meta::Nodes::GetImColorFromNodeType(iterator.second.type_));
				ImGui::Dummy(ImVec2(size, size));
				ImGui::SameLine();
				
				if (ImGui::MenuItem(iterator.first.data()))
				{
					const ImVec2 position = (mouse_ - scroll_ - pos_) / scale_;
				
					nodes_.emplace_back(CreateNode(iterator.first, iterator.second, position));
					break;
				}	
			}
		
			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
	}

	////////////////////////////////////////////////////////////////////////////////

	NodesEditor::NodesEditor() noexcept
		: panel_state_(nodes_)
	{
	}

	void NodesEditor::ProcessPanel(const Editor::Processor&)
	{
		AssertDirtyNodes();

		ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(24, 24, 24, 255));
		ImGui::Begin("NodesEditor");

		Update();
		ProcessNodes();
		ProcessContextMenu();
		ImGui::End();
		ImGui::PopStyleColor();
	}
}