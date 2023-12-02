#pragma once

namespace madX::Editor::Panel
{
	class NodesEditor final : public PanelClass
	{
		typedef unsigned int NodesState;
		enum NodesState_ : NodesState
		{
			NodesState_Default = 0,
			NodesState_HoveringNode,
			NodesState_HoveringOutput,
			NodesState_Draging,
			NodesState_DragingOutput,
			NodesState_Selecting
		};

		float scale_ = 1.0f;
		NodesState state_ = NodesState_Default;

		Meta::Nodes::Node* element_dragging_ = nullptr;
		Meta::Nodes::Node* element_output_ = nullptr;
		Meta::Nodes::Node* element_hovered_ = nullptr;
		Meta::Nodes::Input* element_input_ = nullptr;
		
		Meta::Nodes::Node* processing_node_ = nullptr;

		Meta::Nodes::Node* connection_node_ = nullptr;
		Meta::Nodes::Node* connection_path_ = nullptr;
		Meta::Nodes::Input* connection_input_ = nullptr;

		ImRect area_;

		ImVec2 mouse_;
		ImVec2 pos_;
		ImVec2 size_;
		ImVec2 scroll_;
		ImRect connection_;

		////////////////////////////////////////////////////////////////////////////////

		//TODO: counter 32/32 last/new !!!
		std::uint64_t modified_ = 0;

		inline void MarkModified() noexcept
		{
			++modified_;
		}

		inline void UnMarkModified() noexcept
		{
			if ((modified_ >> 32u) < (modified_ & 0xffffffff))
				modified_ += std::uint64_t(1) << 32u;
		}

		////////////////////////////////////////////////////////////////////////////////

		//Meta::Nodes::NodeDescMap nodes_desc_map_;
		Meta::Nodes::NodesContainer nodes_;

		Core::Filesystem::Stream stream_;

		NodesEditorState panel_state_;

		////////////////////////////////////////////////////////////////////////////////

		void UpdateCanvasGeometry();
		void UpdateNodesOnCanvas();

		inline ImVec2 GetConnectionLineSubPoint(ImVec2 point, Meta::Nodes::ConnectionStyle style)
		{
			if (style & Meta::Nodes::ConnectionStyle_Right)
			{
				point += (ImVec2(Meta::Nodes::NodesConnectionLine, 0.0f) * scale_);
			}
			else if (style & Meta::Nodes::ConnectionStyle_Left)
			{
				point += (ImVec2(-Meta::Nodes::NodesConnectionLine, 0.0f) * scale_);
			}

			if (style & Meta::Nodes::ConnectionStyle_Top)
			{
				point += (ImVec2(0.0f, Meta::Nodes::NodesConnectionLine) * scale_);
			}
			else if (style & Meta::Nodes::ConnectionStyle_Bottom)
			{
				point += (ImVec2(0.0f, -Meta::Nodes::NodesConnectionLine) * scale_);
			}

			return point;
		}

		float GetDistanceToConnection(const ImVec2& p1, const ImVec2& p4, const ImVec2& p, Meta::Nodes::ConnectionStyle begin, Meta::Nodes::ConnectionStyle end)
		{
			ImVec2 p_last = p1;

			const ImVec2 p2 = GetConnectionLineSubPoint(p1, begin);
			const ImVec2 p3 = GetConnectionLineSubPoint(p4, end);

			float p_closest = FLT_MAX;
			float t_step = 1.0f / (float)Meta::Nodes::NodesConnectionSplineSegments;
			for (int i_step = 1; i_step <= Meta::Nodes::NodesConnectionSplineSegments; i_step++)
			{
				ImVec2 p_current = ImBezierCubicCalc(p1, p2, p3, p4, t_step * i_step);
				ImVec2 p_line = ImLineClosestPoint(p_last, p_current, p);
				
				if (float dist = ImLengthSqr(p - p_line); dist < p_closest)
				{
					p_closest = dist;
				}

				p_last = p_current;
			}
			return p_closest;
		}

		inline void DrawConnection(const ImVec2& p1, const ImVec2& p4, ImColor color, Meta::Nodes::ConnectionStyle begin, Meta::Nodes::ConnectionStyle end)
		{
			const ImVec2 p2 = GetConnectionLineSubPoint(p1, begin);
			const ImVec2 p3 = GetConnectionLineSubPoint(p4, end);

		//	float dim = 1.0f;
		//	
		//	if (p1.y > p4.y)
		//	{
		//		dim *= 1.0f - ((p1.y - p4.y) * 0.01);
		//	}
		//	
		//	if (p1.x > p4.x)
		//	{
		//		dim *= 1.0f - ((p1.x - p4.x) * 0.01);
		//	}
		//	
		//	color.Value.w *= dim;

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddBezierCubic(p1, p2, p3, p4, color, scale_, Meta::Nodes::NodesConnectionSplineSegments);
		}

		inline void ClearDirtyFlag() noexcept
		{
			for (auto& node : nodes_)
				node->state_ &= ~Meta::Nodes::NodeStateFlag_Dirty;
		}

		void SplitOrderOnSelectedNodes();
		void SweepRootPaths(Meta::Nodes::Node*);

		inline void PropagateRootedFlag()
		{
			for (auto& node : nodes_)
			{
				if (false == (node->type_ & Meta::Nodes::NodeType_Root))
					continue;

				SweepRootPaths(node.get());
			}
		}

		inline void AssertDirtyNodes() const noexcept
		{
			bool dirty = false;

			for (const auto& node : nodes_)
				dirty |= node->state_ & Meta::Nodes::NodeStateFlag_Dirty ? true : false;

			IM_ASSERT(false == dirty);
		}

		Meta::UniquePtr<Meta::Nodes::Node> CreateNode(const Meta::String&, const Meta::Nodes::NodeDesc&, ImVec2);

		void Update();
		void ProcessNodes();
		void ProcessContextMenu();
	public:
		NodesEditor() noexcept;
		void ProcessPanel(const Editor::Processor&);

		const PanelState& GetPanelState() const noexcept
		{
			return panel_state_;
		}
	};
}