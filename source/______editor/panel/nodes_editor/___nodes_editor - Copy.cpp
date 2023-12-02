#include "_editor.hpp"

#include "nodes_editor.hpp"

namespace madX::Editor::Panel
{
	NodesEditor::NodesEditor() noexcept
	{
		////////////////////////////////////////////////////////////////////////////////

		{
			const auto stream = Core::Filesystem::Stream("nodes/nodes_connector.abi");
			const Meta::Object nodes_connector_abi = Meta::JSON::GetMetaObject(stream);
		
			using namespace Meta::Nodes;

			BuildupABIShiftMap(nodes_connector_abi, "NodeConnectorFlag", cNodeConnectorABI_FlagBitStart, cNodeConnectorABI_FlagBitStop, nodes_connector_abi_);
			BuildupABIShiftMap(nodes_connector_abi, "NodeConnectorValueType", cNodeConnectorABI_ValueTypeBitStart, cNodeConnectorABI_ValueTypeBitStop, nodes_connector_abi_);
			BuildupABIShiftMap(nodes_connector_abi, "NodeConnectorType", cNodeConnectorABI_TypeBitStart, cNodeConnectorABI_TypeBitStop, nodes_connector_abi_);
			BuildupABIShiftMap(nodes_connector_abi, "NodeConnectorTypeMark", cNodeConnectorABI_TypeMarkBitStart, cNodeConnectorABI_TypeMarkBitStop, nodes_connector_abi_);
		
			nodes_connector_abi_.emplace(Meta::String("_null"), std::uint64_t(0));
		}
		
		////////////////////////////////////////////////////////////////////////////////
		
		{
			const auto stream = Core::Filesystem::Stream("nodes/nodes.abi");
			const Meta::Object nodes_abi = Meta::JSON::GetMetaObject(stream);
		
			using namespace Meta::Nodes;

			BuildupABIShiftMap(nodes_abi, "NodeFlag", cNodeABI_FlagBitStart, cNodeABI_FlagBitStop, nodes_abi_);
			BuildupABIShiftMap(nodes_abi, "NodeType", cNodeABI_TypeBitStart, cNodeABI_TypeBitStop, nodes_abi_);
		
			nodes_abi_.emplace(Meta::String("_null"), std::uint64_t(0));
		}

		////////////////////////////////////////////////////////////////////////////////

		auto logger = Core::Logger::GetInstance();

		for (const auto& node_type : nodes_abi_)
		{
			if (const auto node_type_name = node_type.first.view(); node_type_name.starts_with("_"))
			{
				continue;
			}

			const std::string node_path = "nodes/" + node_type.first.str() + ".node";
			const auto stream = Core::Filesystem::Stream(node_path);

			if (stream.IsEmpty())
			{
				//logger->Warning("Node desc not found: " + node_path);
				continue;
			}
			else
			{
				logger->Info("Node desc found: " + node_path);
			}

			const Meta::Object node_json = Meta::JSON::GetMetaObject(stream);
			const auto meta_object = node_json.find(node_type.first);

			if (meta_object == node_json.cend())
			{
				logger->Error("* Missing node desc::name: " + node_type.first.str());
				continue;
			}

			const auto meta_structure = cista::get_if<Meta::MetaArray>(meta_object->second);

			if (meta_structure == nullptr)
			{
				logger->Error("* Wrong node desc type (must be Meta::Array)");
				continue;			
			}

			////////////////////////////////////////////////////////////////////////////////

			for (const auto& meta_structure_element : *meta_structure->get())
			{
				const auto meta_element = cista::get_if<Meta::MetaObject>(meta_structure_element);

				if (meta_element == nullptr)
				{
					logger->Error("* Wrong node desc structure type (must be Meta::MetaObject)");
					break;
				}

				const auto element = meta_element->get();

				Meta::Nodes::NodeDesc node_desc;

				try
				{
					node_desc.type_ = nodes_abi_.at(node_type.first);
				}
				catch (...)
				{
					logger->Error("* Missing node desc abi type: " + node_type.first.str());
					return;
				}


				if (const auto meta_name = element->find("_name"); meta_name != element->cend())
				{
					if (const auto name = cista::get_if<Meta::String>(meta_name->second); name != nullptr)
					{
						node_desc.name_ = *name;
					}
					else
					{
						logger->Error("* Wrong node structure property _name type (must be Meta::String)");
						break;
					}
				}
				else
				{
					logger->Error("* Missing node structure property: _name");
					break;
				}

				////////////////////////////////////////////////////////////////////////////////

				for (const auto& meta_property : *element)
				{
					if (const auto meta_property_name = meta_property.first.view(); meta_property_name.starts_with("_"))
					{
						continue;
					}

					Meta::Nodes::PropertyDesc property_desc;
					property_desc.name_ = meta_property.first;

					const auto meta_connector = cista::get_if<Meta::MetaArray>(meta_property.second);
					if (meta_connector == nullptr)
					{
						logger->Error("* Wrong node structure property type (must be Meta::Array) on: " + property_desc.name_.str());
						break;
					}

					for (const auto& connector : *meta_connector->get())
					{
						if (const auto name = cista::get_if<Meta::String>(connector); name != nullptr)
						{
							Meta::Nodes::ConnectorDesc connector_desc;
							connector_desc.name_ = *name;
							property_desc.connectors_.emplace_back(std::move(connector_desc));
						}
						else
						{
							logger->Error("* Wrong node structure sub-property type (must be Meta::String) on: " + property_desc.name_.str());
							break;
						}						
					}
					
					node_desc.properties_.emplace_back(std::move(property_desc));
				}

				////////////////////////////////////////////////////////////////////////////////

				for (auto& property : node_desc.properties_)
				{
					GetConnectorsDesc(node_json, nodes_connector_abi_, property);
				}

				////////////////////////////////////////////////////////////////////////////////

				nodes_desc_map_.emplace(node_desc.name_, std::move(node_desc));
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		return;
	}

	////////////////////////////////////////////////////////////////////////////////

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

		if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_Home]))
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
						scale_ = ImMin(1.0f, scale_ * 1.05f);

				ImVec2 shift = scroll_ + (focus * scale_);
				scroll_ += mouse_ - shift - pos_;
			}

			if (ImGui::IsMouseReleased(1) && element_node_ == nullptr)
				if (io.MouseDragMaxDistanceSqr[1] < (io.MouseDragThreshold * io.MouseDragThreshold))
				{
					bool selected = false;

					for (int node_idx = 0; node_idx < nodes_.size(); ++node_idx)
					{
						if (nodes_[node_idx]->state_ & Nodes::NodeStateFlag_Selected)
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

	Nodes::Node* NodesEditor::UpdateNodesFromCanvas()
	{
		if (nodes_.empty())
			return nullptr;

		const ImGuiIO& io = ImGui::GetIO();

		ImVec2 offset = pos_ + scroll_;
		ImRect canvas(pos_, pos_ + size_);

		Nodes::Node* hovered_node = nullptr;

		for (auto node_idx = nodes_.size(); node_idx != 0;)
		{
			Nodes::Node* node = nodes_[--node_idx];
			IM_ASSERT(node);

			ImRect node_rect = node->area_node_;
			node_rect.Min *= scale_;
			node_rect.Max *= scale_;
			node_rect.Translate(offset);

			node_rect.ClipWith(canvas);

			if (canvas.Overlaps(node_rect))
			{
				node->state_ |= Nodes::NodeStateFlag_Visible;
				node->state_ &= ~Nodes::NodeStateFlag_Hovered;
			}
			else
			{
				node->state_ &= ~(Nodes::NodeStateFlag_Visible | Nodes::NodeStateFlag_Hovered | Nodes::NodeStateFlag_Marked);
				continue;
			}

			if (nullptr == hovered_node && node_rect.Contains(mouse_))
				hovered_node = node;

			////////////////////////////////////////////////////////////////////////////////

			if (state_ == Nodes::NodesState_Selecting)
			{
				if (io.KeyCtrl && area_.Overlaps(node_rect))
				{
					node->state_ |= Nodes::NodeStateFlag_Marked;
					continue;
				}

				if (false == io.KeyCtrl && area_.Contains(node_rect))
				{
					node->state_ |= Nodes::NodeStateFlag_Marked;
					continue;
				}

				node->state_ &= ~Nodes::NodeStateFlag_Marked;
			}

			////////////////////////////////////////////////////////////////////////////////

			for (Nodes::ConnectorInput& input : node->inputs_)
			{
				if (input.type_ == Meta::Nodes::cConnectorTypeNull)
					continue;

				input.state_ &= ~(Nodes::ConnectorStateFlag_Hovered | Nodes::ConnectorStateFlag_Consider | Nodes::ConnectorStateFlag_Draging);

				if (state_ == Nodes::NodesState_DragingInput)
				{
					if (&input == element_input_)
						input.state_ |= Nodes::ConnectorStateFlag_Draging;

					continue;
				}				

				if (state_ == Nodes::NodesState_DragingOutput)
				{
					if (element_node_ == node)
						continue;

					if (ConnectionMatrix(node, element_node_, &input, element_output_))
						input.state_ |= Nodes::ConnectorStateFlag_Consider;
				}

				if (!hovered_node || hovered_node != node)
					continue;

				if (state_ == Nodes::NodesState_Selecting)
					continue;

				if (state_ != Nodes::NodesState_DragingOutput && node->state_ & Nodes::NodeStateFlag_Selected)
					continue;

				ImRect input_rect = input.area_connector_;
				input_rect.Min *= scale_;
				input_rect.Max *= scale_;
				input_rect.Translate(offset);

				if (input_rect.Contains(mouse_))
				{
					if (state_ != Nodes::NodesState_DragingOutput)
					{
						input.state_ |= Nodes::ConnectorStateFlag_Hovered;
						continue;
					}
					
					if (input.state_ & Nodes::ConnectorStateFlag_Consider)
						input.state_ |= Nodes::ConnectorStateFlag_Hovered;
				}				
			}

			////////////////////////////////////////////////////////////////////////////////
			
			for (Nodes::ConnectorOutput& output : node->outputs_)
			{
				if (output.type_ == Meta::Nodes::cConnectorTypeNull)
					continue;

				output.state_ &= ~(Nodes::ConnectorStateFlag_Hovered | Nodes::ConnectorStateFlag_Consider | Nodes::ConnectorStateFlag_Draging);

				if (state_ == Nodes::NodesState_DragingOutput)
				{
					if (&output == element_output_)
						output.state_ |= Nodes::ConnectorStateFlag_Draging;

					continue;
				}

				if (state_ == Nodes::NodesState_DragingInput)
				{
					if (element_node_ == node)
						continue;

					if (ConnectionMatrix(element_node_, node, element_input_, &output))
						output.state_ |= Nodes::ConnectorStateFlag_Consider;
				}

				if (!hovered_node || hovered_node != node)
					continue;
			
				if (state_ == Nodes::NodesState_Selecting)
					continue;

				if (state_ != Nodes::NodesState_DragingInput && node->state_ & Nodes::NodeStateFlag_Selected)
					continue;
				
				ImRect output_rect = output.area_connector_;
				output_rect.Min *= scale_;
				output_rect.Max *= scale_;
				output_rect.Translate(offset);

				if (output_rect.Contains(mouse_))
				{
					if (state_ != Nodes::NodesState_DragingInput)
					{
						output.state_ |= Nodes::ConnectorStateFlag_Hovered;
						continue;
					}

					if (output.state_ & Nodes::ConnectorStateFlag_Consider)
						output.state_ |= Nodes::ConnectorStateFlag_Hovered;
				}
			}	
		}

		if (hovered_node)
			hovered_node->state_ |= Nodes::NodeStateFlag_Hovered;

		return hovered_node;
	}

	////////////////////////////////////////////////////////////////////////////////

	bool NodesEditor::SortSelectedNodesOrder()
	{
		bool selected = false;

		std::vector<Nodes::Node*> nodes_unselected;
		nodes_unselected.reserve(nodes_.size());

		std::vector<Nodes::Node*> nodes_selected;
		nodes_selected.reserve(nodes_.size());

		for (auto node : nodes_)
		{
			if (node->state_ & Nodes::NodeStateFlag_Marked || node->state_ & Nodes::NodeStateFlag_Selected)
			{
				selected = true;
				node->state_ &= ~Nodes::NodeStateFlag_Marked;
				node->state_ |= Nodes::NodeStateFlag_Selected;
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

	////////////////////////////////////////////////////////////////////////////////
	
	Nodes::Node* NodesEditor::CreateNode(const Meta::Nodes::ShiftMap& abi_map, const Meta::Nodes::NodeDesc& desc, ImVec2 pos)
	{
		Nodes::Node* node = new Nodes::Node(desc.name_.data(), desc.type_, Nodes::GetImColorFromNodeType(desc.type_));

		try
		{
			for (const auto& property : desc.properties_)
			{
				for (const auto& connector : property.connectors_)
				{
					if (connector.type_ & abi_map.at("_input"))
					{
						node->inputs_.emplace_back(connector.name_.data(), connector.type_);
						continue;
					}
		
					if (connector.type_ & abi_map.at("_output"))
					{
						node->outputs_.emplace_back(connector.name_.data(), connector.type_);
						continue;
					}
				}
			}
		}
		catch(...)
		{
			delete node;
			return nullptr;
		}


		ImVec2 inputs;
		ImVec2 outputs;

		////////////////////////////////////////////////////////////////////////////////

		for (const auto& input : node->inputs_)
		{	
			inputs.x = ImMax(inputs.x, input.area_connector_.GetWidth());
			inputs.y += input.area_connector_.GetHeight();	
		}
		
		for (const auto& output : node->outputs_)
		{	
			outputs.x = ImMax(outputs.x, output.area_connector_.GetWidth());
			outputs.y += output.area_connector_.GetHeight();
		}

		////////////////////////////////////////////////////////////////////////////////

		node->BuildGeometry(inputs, outputs);
		node->TranslateNode(pos - node->area_node_.GetCenter());
		node->state_ |= Nodes::NodeStateFlag_Visible | Nodes::NodeStateFlag_Hovered | Nodes::NodeStateFlag_Processing;

		////////////////////////////////////////////////////////////////////////////////

		if (processing_node_)
			processing_node_->state_ &= ~(Nodes::NodeStateFlag_Processing);

		return processing_node_ = node;
	}	

	////////////////////////////////////////////////////////////////////////////////

	void NodesEditor::Update()
	{
		const ImGuiIO& io = ImGui::GetIO();
		
		UpdateCanvasGeometry();

		if (false == ImGui::IsWindowFocused())
		{
			return;
		}

		////////////////////////////////////////////////////////////////////////////////

		Nodes::Node* hovered_node = UpdateNodesFromCanvas();

		bool consider_hover = state_ == Nodes::NodesState_Default;
		consider_hover |= state_ == Nodes::NodesState_HoveringNode;
		consider_hover |= state_ == Nodes::NodesState_HoveringInput;
		consider_hover |= state_ == Nodes::NodesState_HoveringOutput;

		////////////////////////////////////////////////////////////////////////////////

		if (hovered_node && consider_hover)
		{
			element_input_ = nullptr;
			element_output_ = nullptr;

			for (int input_idx = 0; input_idx < hovered_node->inputs_.size(); ++input_idx)
			{
				if (hovered_node->inputs_[input_idx].state_ & Nodes::ConnectorStateFlag_Hovered)
				{
					element_input_ = &hovered_node->inputs_[input_idx];
					state_ = Nodes::NodesState_HoveringInput;
					break;
				}
			}						
			
			for (int output_idx = 0; output_idx < hovered_node->outputs_.size(); ++output_idx)
			{
				if (hovered_node->outputs_[output_idx].state_ & Nodes::ConnectorStateFlag_Hovered)
				{
					element_output_ = &hovered_node->outputs_[output_idx];
					state_ = Nodes::NodesState_HoveringOutput;
					break;
				}
			}					

			if (!element_input_ && !element_output_)
				state_ = Nodes::NodesState_HoveringNode;	
		}

		////////////////////////////////////////////////////////////////////////////////

		if (state_ == Nodes::NodesState_DragingInput)
		{
			element_output_ = nullptr;
		
			if (hovered_node)
				for (int output_idx = 0; output_idx < hovered_node->outputs_.size(); ++output_idx)
				{
					Nodes::ConnectorState state = hovered_node->outputs_[output_idx].state_;

					if (state & Nodes::ConnectorStateFlag_Hovered && state & Nodes::ConnectorStateFlag_Consider)
						element_output_ = &hovered_node->outputs_[output_idx];
				}
		}
		
		if (state_ == Nodes::NodesState_DragingOutput)
		{
			element_input_ = nullptr;
		
			if (hovered_node)
				for (int input_idx = 0; input_idx < hovered_node->inputs_.size(); ++input_idx)
				{
					Nodes::ConnectorState state = hovered_node->inputs_[input_idx].state_;

					if (state & Nodes::ConnectorStateFlag_Hovered && state & Nodes::ConnectorStateFlag_Consider)
						element_input_ = &hovered_node->inputs_[input_idx];
				}
		}

		////////////////////////////////////////////////////////////////////////////////

		if (consider_hover)
		{
			element_node_ = hovered_node;

			if (!hovered_node)
				state_ = Nodes::NodesState_Default;
		}					

		////////////////////////////////////////////////////////////////////////////////
	
		if (ImGui::IsMouseDoubleClicked(0))
		{
			switch (state_)
			{
				case Nodes::NodesState_Default:
				{
					bool selected = false;

					for (int node_idx = 0; node_idx < nodes_.size(); ++node_idx)
					{
					    Nodes::NodeState& state = nodes_[node_idx]->state_;

					    if (state & Nodes::NodeStateFlag_Selected)
						    selected = true;

						state &= ~(Nodes::NodeStateFlag_Selected | Nodes::NodeStateFlag_Marked | Nodes::NodeStateFlag_Hovered);
					}

					if (processing_node_ && false == selected)
					{
						processing_node_->state_ &= ~(Nodes::NodeStateFlag_Processing);
						processing_node_ = nullptr;
					}				  

					return;
				};
	
				case Nodes::NodesState_HoveringInput:
			    {
					if (element_input_->target_)
					{
						element_input_->output_->connections_--;
					    element_input_->output_ = nullptr;
						element_input_->target_ = nullptr;

						state_ = Nodes::NodesState_DragingInput;
					}

					return;
				}

				case Nodes::NodesState_HoveringNode:
				{
				    IM_ASSERT(element_node_);

					if (element_node_->inputs_.empty() && element_node_->outputs_.empty())
				    {
						state_ = Nodes::NodesState_Draging;
						return;			    
					}

					if (element_node_->state_ & Nodes::NodeStateFlag_Collapsed)
					{					
						element_node_->state_ &= ~Nodes::NodeStateFlag_Collapsed;
						element_node_->area_node_.Max.y += element_node_->body_height_;

						const auto center = element_node_->area_node_.GetCenter().x;

						const auto width = element_node_->collapsed_width_ * 0.5f;
						element_node_->area_node_.Min.x = center - width;
						element_node_->area_node_.Max.x = center + width;

						element_node_->collapsed_width_ = ImGui::CalcTextSize(element_node_->name_).x;
					}
					else
					{
						element_node_->state_ |= Nodes::NodeStateFlag_Collapsed;
						element_node_->area_node_.Max.y -= element_node_->body_height_;
						
						const auto width = element_node_->area_node_.GetWidth();

						const auto center = element_node_->area_node_.GetCenter().x;
						element_node_->area_node_.Min.x = center - element_node_->collapsed_width_;
						element_node_->area_node_.Max.x = center + element_node_->collapsed_width_;

						element_node_->collapsed_width_ = width;
					}
	
					state_ = Nodes::NodesState_Draging;
					return;
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////
		
		if (ImGui::IsMouseDoubleClicked(1))
		{
			switch (state_)
			{
				case Nodes::NodesState_HoveringNode:
				{
					IM_ASSERT(hovered_node);

					if (hovered_node->state_ & Nodes::NodeStateFlag_Disabled)
						hovered_node->state_ &= ~(Nodes::NodeStateFlag_Disabled);
					else
						hovered_node->state_ |= (Nodes::NodeStateFlag_Disabled);

					return;
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		if (ImGui::IsMouseClicked(0))
		{
			switch (state_)
			{				
				case Nodes::NodesState_HoveringNode:
				{
					if (io.KeyCtrl)
						element_node_->state_ ^= Nodes::NodeStateFlag_Selected;

					if (io.KeyShift)
						element_node_->state_ |= Nodes::NodeStateFlag_Selected;

					bool selected = element_node_->state_ & Nodes::NodeStateFlag_Selected;

					if (false == selected)
					{
						if (processing_node_)
							processing_node_->state_ &= ~(Nodes::NodeStateFlag_Processing);

						element_node_->state_ |= Nodes::NodeStateFlag_Processing;
						processing_node_ = element_node_;

						IM_ASSERT(false == nodes_.empty());

						if (nodes_.back() != element_node_)
						{
							auto found = std::find(nodes_.cbegin(), nodes_.cend(), element_node_);
							nodes_.erase(found);
							nodes_.push_back(element_node_);
						}
					}
					else
						SortSelectedNodesOrder();
	
					state_ = Nodes::NodesState_Draging;
					return;
				}
	
				case Nodes::NodesState_HoveringInput:
				{
					if (!element_input_->target_)
						state_ = Nodes::NodesState_DragingInput;
					else
						state_ = Nodes::NodesState_Draging;

					return;
				}
	
				case Nodes::NodesState_HoveringOutput:
				{
					state_ = Nodes::NodesState_DragingOutput;
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
				case Nodes::NodesState_Default:
				{
					ImRect canvas(pos_, pos_ + size_);
					if (false == canvas.Contains(mouse_))
						return;
	
					if (false == io.KeyShift)
						for (int node_idx = 0; node_idx < nodes_.size(); ++node_idx)
							nodes_[node_idx]->state_ &= ~(Nodes::NodeStateFlag_Selected | Nodes::NodeStateFlag_Marked);
	
					state_ = Nodes::NodesState_Selecting;
					return;
				}
	
				case Nodes::NodesState_Selecting:
				{
					const ImVec2 pos = mouse_ - ImGui::GetMouseDragDelta(0);
	
					area_.Min = ImMin(pos, mouse_);
					area_.Max = ImMax(pos, mouse_);
	
					return;
				}
	
				case Nodes::NodesState_Draging:
				{
					if (element_input_ && element_input_->output_ && element_input_->output_->connections_ > 0)
				        return;

					if (false == (element_node_->state_ & Nodes::NodeStateFlag_Selected))
						element_node_->TranslateNode(io.MouseDelta / scale_, false);
					else
						for (int node_idx = 0; node_idx < nodes_.size(); ++node_idx)
							nodes_[node_idx]->TranslateNode(io.MouseDelta / scale_, true);
	
					return;
				}
	
				case Nodes::NodesState_DragingInput:
				{
					ImVec2 offset = pos_ + scroll_;
					ImVec2 p1 = offset + (element_input_->pos_ * scale_);
					ImVec2 p4 = element_output_ ? (offset + (element_output_->pos_ * scale_)) : mouse_;
	
					connection_ = ImVec4(p1.x, p1.y, p4.x, p4.y);
					return;
				}
	
				case Nodes::NodesState_DragingOutput:
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
			case Nodes::NodesState_Selecting:
			{
				element_node_ = nullptr;
				element_input_ = nullptr;
				element_output_ = nullptr;

				area_ = {};

				////////////////////////////////////////////////////////////////////////////////

				SortSelectedNodesOrder();
				state_ = Nodes::NodesState_Default;
				return;
			}

			case Nodes::NodesState_Draging:
			{
				state_ = Nodes::NodesState_HoveringNode;
				return;
			}

			case Nodes::NodesState_DragingInput:
			case Nodes::NodesState_DragingOutput:
			{
				if (element_input_ && element_output_)
				{
					IM_ASSERT(hovered_node);
					IM_ASSERT(element_node_);
					element_input_->target_ = state_ == Nodes::NodesState_DragingInput ? hovered_node : element_node_;

					if (element_input_->output_)
						element_input_->output_->connections_--;

					element_input_->output_ = element_output_;
					element_output_->connections_++;
				}

				connection_ = ImVec4();
				state_ = Nodes::NodesState_Default;
				return;
			}
			}

			return;
		}

		////////////////////////////////////////////////////////////////////////////////
		
		if (ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_Delete]))
		{
			std::vector<Nodes::Node*> nodes;
			nodes.reserve(nodes_.size());

			for (int node_idx = 0; node_idx < nodes_.size(); ++node_idx)
			{
				Nodes::Node* node = nodes_[node_idx];
				IM_ASSERT(node);

				if (node->state_ & Nodes::NodeStateFlag_Selected)
				{
					element_node_ = nullptr;
					element_input_ = nullptr;
					element_output_ = nullptr;

					state_ = Nodes::NodesState_Default;

					for (int sweep_idx = 0; sweep_idx < nodes_.size(); ++sweep_idx)
					{
						Nodes::Node* sweep = nodes_[sweep_idx];
						IM_ASSERT(sweep);
					
						for (int input_idx = 0; input_idx < sweep->inputs_.size(); ++input_idx)
						{
							Nodes::ConnectorInput& input = sweep->inputs_[input_idx];

							if (node == input.target_)
							{
								if (input.output_)
									input.output_->connections_--;

								input.target_ = nullptr;
								input.output_ = nullptr;
							}
						}
					}

					for (int input_idx = 0; input_idx < node->inputs_.size(); ++input_idx)
					{
						Nodes::ConnectorInput& input = node->inputs_[input_idx];
						
						if (input.output_)
							input.output_->connections_--;
						
						input.type_ = Meta::Nodes::cConnectorTypeNull;
						input.name_ = nullptr;
						input.target_ = nullptr;
						input.output_ = nullptr;
					}

					for (int output_idx = 0; output_idx < node->outputs_.size(); ++output_idx)
					{
						Nodes::ConnectorOutput& output = node->outputs_[output_idx];
						IM_ASSERT(output.connections_ == 0);
					}

					if (node == processing_node_)
						processing_node_ = nullptr;
		
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

	////////////////////////////////////////////////////////////////////////////////

	void NodesEditor::ProcessNodes()
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		const ImVec2 offset = pos_ + scroll_;

		////////////////////////////////////////////////////////////////////////////////

		ImGui::SetWindowFontScale(scale_);

		for (int node_idx = 0; node_idx < nodes_.size(); ++node_idx)
		{
			const Nodes::Node* node = nodes_[node_idx];
			IM_ASSERT(node);

			for (int input_idx = 0; input_idx < node->inputs_.size(); ++input_idx)
			{		
				const Nodes::ConnectorInput& input = node->inputs_[input_idx];
				
				if (const Nodes::Node* target = input.target_)
				{
					IM_ASSERT(target);

					ImVec2 p1 = offset;
					ImVec2 p4 = offset;

					if (node->state_ & Nodes::NodeStateFlag_Collapsed)
					{
						ImVec2 collapsed_input = { 0, (node->area_node_.Max.y - node->area_node_.Min.y) * 0.5f };					

						p1 += ((node->area_node_.Min + collapsed_input) * scale_);
					}
					else
					{
						p1 += (input.pos_ * scale_);
					}

					if (target->state_ & Nodes::NodeStateFlag_Collapsed)
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
			const Nodes::Node* node = nodes_[node_idx];
			IM_ASSERT(node);
			node->Draw(draw_list, offset, scale_, state_);
		}

		if (connection_.x != connection_.z && connection_.y != connection_.w)
			DrawConnection(ImVec2(connection_.x, connection_.y), ImVec2(connection_.z, connection_.w), ImColor(0.0f, 1.0f, 0.0f, 1.0f));

		ImGui::SetWindowFontScale(1.0f);

		////////////////////////////////////////////////////////////////////////////////

		if (state_ == Nodes::NodesState_Selecting)
		{
			draw_list->AddRectFilled(area_.Min, area_.Max, ImColor(1.0f, 1.0f, 0.0f, 0.1f));
			draw_list->AddRect(area_.Min, area_.Max, ImColor(1.0f, 1.0f, 0.0f, 0.5f));
		}

		////////////////////////////////////////////////////////////////////////////////

		ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
		ImGui::NewLine();
		
		switch (state_)
		{
			case Nodes::NodesState_Default: ImGui::Text("Nodes::NodesState_Default"); break;
			case Nodes::NodesState_HoveringNode: ImGui::Text("Nodes::NodesState_HoveringNode"); break;
			case Nodes::NodesState_HoveringInput: ImGui::Text("Nodes::NodesState_HoveringInput"); break;
			case Nodes::NodesState_HoveringOutput: ImGui::Text("Nodes::NodesState_HoveringOutput"); break;
			case Nodes::NodesState_Draging: ImGui::Text("Nodes::NodesState_Draging"); break;
			case Nodes::NodesState_DragingInput: ImGui::Text("Nodes::NodesState_DragingInput"); break;
			case Nodes::NodesState_DragingOutput: ImGui::Text("Nodes::NodesState_DragingOutput"); break;
			case Nodes::NodesState_Selecting: ImGui::Text("Nodes::NodesState_Selecting"); break;
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
	}

	////////////////////////////////////////////////////////////////////////////////

	void NodesEditor::ProcessContextMenu()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

		if (ImGui::BeginPopup("NodesContextMenu"))
		{
			const float size = ImGui::GetTextLineHeight();

			for (const auto& iterator : nodes_desc_map_)
			{
				assert(iterator.second.type_ & Meta::Nodes::cNodeABI_TypeMask);

				ImVec2 pos = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size, pos.y + size), Nodes::GetImColorFromNodeType(iterator.second.type_));
				ImGui::Dummy(ImVec2(size, size));
				ImGui::SameLine();

				if (ImGui::MenuItem(iterator.first.data()))
				{
					ImVec2 position = (mouse_ - scroll_ - pos_) / scale_;

					Nodes::Node* node = CreateNode(nodes_connector_abi_, iterator.second, position);

					auto registry = ECS::Registry::GetInstance();
					const auto entity = registry->Create();
					
					//node->entity_ = entity;
					nodes_.push_back(node);

					registry->Emplace<Meta::Nodes::Node*>(entity, node);
				}	
			}
		
			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
	}

	////////////////////////////////////////////////////////////////////////////////

	void NodesEditor::RenderPanel(const std::shared_ptr<Root::Component::Project>& project)
	{
		ImGui::Begin("NodesEditor");

		Update();
		ProcessNodes();
		ProcessContextMenu();
		ImGui::End();
	}
}