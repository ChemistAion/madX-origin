import <cassert>;

import stl.core;

namespace madX::Core
{
	class Graph;
	class GraphNode;
	class GraphEdge;

	using GraphNodeID = std::uint32_t;
	using GraphNodeCount = std::uint32_t;

	using GraphEdges = std::vector<GraphEdge*>;

	static constexpr std::uint32_t cGraphTarget = 0x80000000;

	////////////////////////////////////////////////////////////////////////////////

	//TODO: isvalid
	class GraphNode
	{
		friend Graph;
		const GraphNodeID id_ = 0;
		GraphNodeCount count_ = 0;
	public:
		GraphNode(Graph&);

		[[nodiscard]] const GraphNodeID GetID() const noexcept
		{
			return id_;
		}

		void MarkAsTarget() noexcept
		{
			count_ = cGraphTarget;
		}

		bool IsTarget() const noexcept
		{
			return count_ >= cGraphTarget;
		}

		bool IsCulled() const noexcept
		{
			return count_ == 0;
		}

		const GraphNodeCount GetCount() const noexcept
		{
			return IsTarget() ? 1 : count_;
		}
	};

	////////////////////////////////////////////////////////////////////////////////

	//TODO: isvalid
	class GraphEdge
	{
		friend Graph;
		const GraphNodeID from_ = 0;
		const GraphNodeID to_ = 0;
	public:
		GraphEdge(Graph&, GraphNode*, GraphNode*);

		[[nodiscard]] const GraphNodeID GetNodeFrom() const noexcept
		{
			return from_;
		}

		[[nodiscard]] const GraphNodeID GetNodeTo() const noexcept
		{
			return to_;
		}
	};

	////////////////////////////////////////////////////////////////////////////////

	class Graph
	{
		std::vector<GraphNode*> nodes_;
		std::vector<GraphEdge*> edges_;
	public:
		[[nodiscard]] const GraphNodeID CreateNodeID() const noexcept
		{
			//TODO: test-check it
			return (GraphNodeID)nodes_.size();
		}

		//TODO: danger! accessing a nonexistent element through this operator is undefined behavior
		[[nodiscard]] GraphNode* GetNode(GraphNodeID id) const noexcept
		{
			return nodes_[id];
		}

		[[nodiscard]] GraphEdge* GetEdge(GraphNodeID, GraphNodeID) const noexcept;

		void RegisterNode(GraphNode* node) noexcept
		{
			assert(node);
			assert(node->GetID() == (GraphNodeID)nodes_.size());
			nodes_.push_back(node);
		}

		void RegisterEdge(GraphEdge* edge) noexcept
		{
			assert(edge);
			edges_.push_back(edge);
		}

		void Clear() noexcept
		{
			nodes_.clear();
			edges_.clear();
		}

		void Cull() noexcept;

		bool IsEdgeValid(const GraphEdge*) const noexcept;
		bool IsNodeValid(const GraphNode*) const noexcept;

		void GetIncomingEdges(const GraphNode*, GraphEdges&) const noexcept;
		void GetOutgoingEdges(const GraphNode*, GraphEdges&) const noexcept;
	};
}