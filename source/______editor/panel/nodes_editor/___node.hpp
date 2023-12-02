#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_internal.h"

namespace Nodes
{
	typedef unsigned int ConnectionStyle;
	enum ConnectionStyle_ : ConnectionStyle
	{
		ConnectionStyle_None = 0,
		ConnectionStyle_Right = 1 << 0,
		ConnectionStyle_Left = 1 << 1,
		ConnectionStyle_Top = 1 << 2,
		ConnectionStyle_Bottom = 1 << 3
	};

	////////////////////////////////////////////////////////////////////////////////

	typedef unsigned int InputState;
	enum InputStateFlag_ : InputState
	{
		InputStateFlag_Default = 0,
		InputStateFlag_Hovered = 1 << 0
	};

	////////////////////////////////////////////////////////////////////////////////

	typedef unsigned int NodeState;
	enum NodeStateFlag_ : NodeState
	{
		NodeStateFlag_Default = 0,
		NodeStateFlag_Visible = 1 << 0,
		NodeStateFlag_Rooted = 1 << 1,
		NodeStateFlag_Marked = 1 << 2,
		NodeStateFlag_Selected = 1 << 3,
		NodeStateFlag_TabRight = 1 << 4,
		NodeStateFlag_TabTop = 1 << 5,
		NodeStateFlag_Processing = 1 << 6,
		NodeStateFlag_Dirty = 1 << 7
	};

	////////////////////////////////////////////////////////////////////////////////

	typedef unsigned int NodesState;
	enum NodesState_ : NodesState
	{
		NodesState_Default = 0,
		NodesState_HoveringNode,
		//NodesState_HoveringInput,
		NodesState_HoveringOutput,
		NodesState_Draging,
		//NodesState_DragingInput,
		NodesState_DragingOutput,
		NodesState_Selecting
	};

	////////////////////////////////////////////////////////////////////////////////

	struct Node;

	constexpr float NodesConnectionLine = 50.0f;
	constexpr float NodesRoundingRadius = 5.0f;
	constexpr float NodesInputSeparationFactor = 2.0f;
	constexpr int NodesInputDrawSegments = 6;
	constexpr int NodesConnectionSplineSegments = 16;
	constexpr float NodesDistanceToConnection = 20.0f;
	constexpr float NodesSnap = 10.0f;
	constexpr float NodesSpacing = 15.0f;

	////////////////////////////////////////////////////////////////////////////////

	inline ImColor GetImColorFromNodeType(Meta::Nodes::NodeType type)
	{
		unsigned long index;
		_BitScanForward64(&index, type & Meta::Nodes::cNodeABI_TypeMask);
		
		assert(index >= Meta::Nodes::cNodeABI_TypeBitStart);
		assert(index <= Meta::Nodes::cNodeABI_TypeBitStop);
		
		index -= Meta::Nodes::cNodeABI_TypeBitStart;
		
		return ImColor::HSV(index / (float)(Meta::Nodes::cNodeABI_TypeCount), 0.5f, 0.5f);
	}

	////////////////////////////////////////////////////////////////////////////////

	//both parameters are scaled already
	inline ImVec2 GetInputPos(const ImRect& body, float rounding) noexcept
	{
		return ImVec2(body.Min.x + rounding * NodesInputSeparationFactor, body.Max.y);
	}

	//both parameters are scaled already
	inline ImVec2 GetInputSeparation(float rounding) noexcept
	{
		return ImVec2(rounding * NodesInputSeparationFactor, 0.0f);
	}

	//both parameters are scaled already
	inline ImRect GetInputArea(const ImVec2& pos, float rounding) noexcept
	{
		return ImRect(pos.x - rounding, pos.y - rounding, pos.x + rounding, pos.y + rounding); 
	}

	struct Input
	{
		Meta::String name_;
		Meta::Ptr<Nodes::Node> source_ = nullptr;
		InputState state_ = InputStateFlag_Default;
	};

	////////////////////////////////////////////////////////////////////////////////

	inline ImVec2 GetSnapModulo(const ImVec2 pos) noexcept
	{
		return ImVec2(std::fmod(pos.x, Nodes::NodesSnap * 2.0), std::fmod(pos.y, Nodes::NodesSnap * 2.0f));
	}

	inline ImVec2 GetSnapPosition(const ImVec2 pos) noexcept
	{
		return pos - GetSnapModulo(pos);
	}

	//inline ImRect GetImRectOnConvas(const ImVec2& pos, const ImRect& rect, const ImVec2& offset, float scale, float separator) noexcept
	inline ImRect GetImRectOnConvas(const ImVec2 pos, const ImRect& rect, const ImVec2 offset, float scale) noexcept
	{
		ImRect result = rect;
		
		result.Translate(GetSnapPosition(pos));

		result.Min *= scale;
		result.Max *= scale;
		result.Translate(offset);

		return result;
	}

	struct Node
	{
		ImVec2 pos_;
		ImVec2 snap_;
		//TODO: move area_xxx as const to ABI
		ImRect area_node_;
		ImRect area_body_;
		ImRect area_title_;
		ImRect area_parent_;
		ImRect area_out_;

		NodeState state_;
		ImColor color_;

		Meta::Nodes::NodeType type_ = Meta::Nodes::cNodeTypeNull;
		Meta::String name_;

		Meta::Vector<Meta::Ptr<Nodes::Node>> paths_;
		Meta::Vector<Input> inputs_;

		inline void TranslateNode(const ImVec2& delta, bool selected_only = false) noexcept
		{
			if (selected_only && false == (state_ & NodeStateFlag_Selected))
				return;

			pos_ += delta;

			ImVec2 snap = snap_;
			snap_ = GetSnapPosition(pos_);

			if (snap.x != snap_.x || snap.y != snap_.y)
			{
				state_ |= NodeStateFlag_Dirty;
			}
		}

		void Draw(ImDrawList*, ImVec2, float, NodesState) const;

		void BuildNode(const Meta::String& name, Meta::Nodes::NodeType type, ImColor color) noexcept;
	};
}

	////////////////////////////////////////////////////////////////////////////////

	template <typename Context>
	void serialize(Context& context, const ImVec2* element, const cista::offset_t)
	{
		context.write(element, (cista::offset_t)sizeof(ImVec2));
	}
	
	template <typename Context>
	void serialize(Context& context, const ImRect* element, const cista::offset_t)
	{
		context.write(element, (cista::offset_t)sizeof(ImRect));
	}
	
	template <typename Context>
	void serialize(Context& context, const ImColor* element, const cista::offset_t)
	{
		context.write(element, (cista::offset_t)sizeof(ImVec4));
	}