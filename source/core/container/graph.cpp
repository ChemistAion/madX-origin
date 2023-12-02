#include "graph.hpp"

namespace madX::Core
{
	GraphEdge::GraphEdge(Graph& graph, GraphNode* from, GraphNode* to)
		: from_(from->GetID())
		, to_(to->GetID())
	{
		graph.RegisterEdge(this);
	}

	////////////////////////////////////////////////////////////////////////////////

	GraphNode::GraphNode(Graph& graph)
		: id_(graph.CreateNodeID())
	{
		graph.RegisterNode(this);
	}

	////////////////////////////////////////////////////////////////////////////////

	GraphEdge* Graph::GetEdge(GraphNodeID from, GraphNodeID to) const noexcept
	{
		//TODO: make it algo-lambda
		//TODO: consider sparse-set/map
		for (auto index = 0; index < edges_.size(); ++index)
		{
			if (edges_[index]->from_ == from && edges_[index]->to_ == to)
			{
				return edges_[index];
			}
		}

		return nullptr;
	}

	void Graph::Cull() noexcept
	{
		//TODO: "update reference counts"
		for (auto index = 0; index < edges_.size(); ++index)
		{
			auto edge = edges_[index];
			auto node = nodes_[edge->from_];
			++node->count_;
		}
		
		//TODO: "cull nodes with a 0 reference count"
		std::vector<GraphNode*> stack;
		for (auto index = 0; index < nodes_.size(); ++index)
		{
			if (nodes_[index]->GetCount() == 0)
			{
				stack.push_back(nodes_[index]);
			}
		}
		
		while (false == stack.empty())
		{
			auto node = stack.back();
			stack.pop_back();
		
			std::vector<GraphEdge*> incoming;
			GetIncomingEdges(node, incoming);
		
			for (auto index = 0; index < incoming.size(); ++index)
			{
				auto linked_node = GetNode(incoming[index]->from_);
		
				if (--linked_node->count_ == 0)
				{
					stack.push_back(linked_node);
				}
			}
		}
	}

	bool Graph::IsEdgeValid(const GraphEdge* edge) const noexcept
	{
		const auto from = GetNode(edge->from_)->IsCulled();
		const auto to = GetNode(edge->to_)->IsCulled();
		
		return (false == from) && (false == to);
	}

	bool Graph::IsNodeValid(const GraphNode* node) const noexcept
	{
		return false;
	}

	void Graph::GetIncomingEdges(const GraphNode* node, GraphEdges& edges) const noexcept
	{
		edges.clear();

		const auto id = node->GetID();

		for (auto index = 0; index < edges_.size(); ++index)
		{
			if (edges[index]->to_ == id)
			{
				edges.push_back(edges_[index]);
			}
		}
	}

	void Graph::GetOutgoingEdges(const GraphNode* node, GraphEdges& edges) const noexcept
	{
		edges.clear();

		const auto id = node->GetID();

		for (auto index = 0; index < edges_.size(); ++index)
		{
			if (edges[index]->from_ == id)
			{
				edges.push_back(edges_[index]);
			}
		}
	}
}