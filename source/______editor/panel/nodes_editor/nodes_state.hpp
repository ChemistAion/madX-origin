#pragma once

namespace madX::Editor::Panel
{
	struct NodesEditorState : public PanelState
	{
		const Meta::Nodes::NodesContainer& nodes_;
		Meta::Nodes::Node* processing_node_ = nullptr;

		NodesEditorState(const Meta::Nodes::NodesContainer& nodes)
			: PanelState("Nodes")
			, nodes_{ nodes }
		{
		};
	};
}