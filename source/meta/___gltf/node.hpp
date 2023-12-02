#pragma once

namespace madX::Meta::glTF
{
	struct Transform
	{
		XMFLOAT3 translation_ = { 0.0f, 0.0f, 0.0f };
		XMFLOAT4 rotation_ = { 0.0f, 0.0f, 0.0f, 1.0f };
		XMFLOAT3 scale_ = { 1.0f, 1.0f, 1.0f };
	};

	struct Node
	{
		std::optional<Index> camera_;
		std::vector<Index> children_;
		std::optional<Index> skin_;
		std::variant<XMMATRIX, Transform> transform_;
		std::optional<Index> mesh_;
		//std::vector<float> weights_;

		Name name_;
		Extras extras_;

		std::optional<Index> light_;
		//std::optional<Index> parent_;
	};

	using Nodes = std::vector<Node>;
	Nodes GetNodes(const simdjson::dom::element& element);
}