#include "_editor.hpp"

#include "properties.hpp"

namespace madX::Editor::Panel
{
	void Properties::ProcessPanel(const Panel::NodesEditorState& state)
	{
		ImGui::Begin("Properties", NULL);

		const auto& nodes = state.nodes_;

		{
			bool dirty = false;

			for (const auto& node : nodes)
				dirty |= node->state_ & Meta::Nodes::NodeStateFlag_Dirty ? true : false;

			IM_ASSERT(false == dirty);
		}

		////////////////////////////////////////////////////////////////////////////////

		auto print_node = [](auto&& node)
		{
			assert(node->type_ & Meta::Nodes::cNodeABI_TypeMask);

			const float size = ImGui::GetTextLineHeight();
			const ImVec2 pos = ImGui::GetCursorScreenPos();

			ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size, pos.y + size), Meta::Nodes::GetImColorFromNodeType(node->type_));
			ImGui::Dummy(ImVec2(size, size));
			ImGui::SameLine();
			ImGui::Text("%s", node->name_.data());
			return;
		};

		////////////////////////////////////////////////////////////////////////////////

		ImGui::Text("Nodes size: %d", nodes.size());

		ImGui::NewLine();

		ImGui::Text("Processing node: ");
		ImGui::SameLine();
		if (state.processing_node_)
			print_node(state.processing_node_);
		else
			ImGui::Text("NONE");

		ImGui::NewLine();

		struct Depth
		{
			Meta::Nodes::NodesPaths::const_iterator begin_;
			Meta::Nodes::NodesPaths::const_iterator end_;
			Meta::Nodes::NodesPaths::iterator path_;
		};

		std::vector<Depth> linear;

		for (const auto& node : nodes)
		{
			if (false == (node->type_ & Meta::Nodes::NodeType_Root))
				continue;

			if (node->paths_.empty())
				continue;

			Meta::Nodes::Node* root = node.get();
			linear.emplace_back(root->paths_.begin(), root->paths_.end(), root->paths_.begin());

			while (false == linear.empty())
			{	
				Depth& back = linear.back();
				
				if (back.path_ == back.begin_)
					print_node(root);
			
				if (back.path_ != back.end_)
				{
					root = *back.path_;

					if (false == root->paths_.empty())
						back.path_++;

					linear.emplace_back(root->paths_.begin(), root->paths_.end(), root->paths_.begin());
					continue;
				}
			
				linear.pop_back();
			
				if (false == linear.empty())
				{
					if (linear.back().path_ != linear.back().end_)
						linear.back().path_++;			
				}
			}
		}

		ImGui::End();
	}
}



//			Meta::Nodes::Node* root = node.get();
//			//depth.push_back(std::make_pair<>(root, root->paths_.begin()));
//			depth.emplace_back(root, root->paths_.begin());
//			
//			while (root)
//			{
//				print_node(root);
//			
//				auto& back = depth.back();
//				auto end = back.first->paths_.end();
//				auto path_it = back.second;
//				auto path = *back.second;
//			
//				depth.emplace_back((Meta::Nodes::Node*)*path_it, (Meta::Nodes::NodesPaths::iterator)(++path_it));
//			
//				if (path_it != end)
//				{
//					print_node(*path_it);
//			
//					if ((*path_it)->paths_.empty())
//						break;
//			
//					root = *path_it;
//					//depth.push_back(std::make_pair<>((*path), (++path)));
//					//depth.emplace_back((*path), (++path));
//				}
//			
//				if (depth.empty())
//					root = nullptr;
//			}