#include "_editor.hpp"

#include "panel/nodes_editor/nodes_state.hpp"

namespace madX::Editor
{
	void Processor::ProcessNodes(const Panel::PanelState& state)
	{
		const auto& nodes_state = (const Panel::NodesEditorState&)state;

		const auto& nodes = nodes_state.nodes_;
		
		{
			bool dirty = false;
		
			for (const auto& node : nodes)
				dirty |= node->state_ & Meta::Nodes::NodeStateFlag_Dirty ? true : false;
		
			IM_ASSERT(false == dirty);
		}
		
		processing_node_ = nodes_state.processing_node_;
		
		////////////////////////////////////////////////////////////////////////////////
		
		linear_root_ = Meta::Nodes::BuildLinearRootPaths(nodes);
	}
}
