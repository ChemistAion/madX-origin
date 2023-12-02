#pragma once

namespace madX::Editor
{
	class Processor
	{
		Meta::Nodes::LinearRoot linear_root_;
		Meta::Nodes::Node* processing_node_ = nullptr;

	public:
		Processor() = default;

		void ProcessNodes(const Panel::PanelState&);

		////////////////////////////////////////////////////////////////////////////////

		const auto& GetLinearRoot() const noexcept
		{
			return linear_root_;
		}

		const auto GetProcessingNode() const noexcept
		{
			return processing_node_;
		}
	};
}