#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_internal.h"

#include "state.hpp"

namespace Nodes
{
//	struct Connector : public Meta::Nodes::Connector
//	{
//		ImVec2 pos_ = {};
//		ImRect area_connector_ = {};
//		ImRect area_name_ = {};
//		ConnectorState state_;
//	
//		inline void TranslateConnector(ImVec2 delta)
//		{
//			pos_ += delta;
//			area_connector_.Translate(delta);
//			area_name_.Translate(delta);
//		}
//	};
//	
//	//TODO: combine input and output
//	struct ConnectorInput : public Connector
//	{
//		Node* target_ = nullptr;
//		ConnectorOutput* output_ = nullptr;
//	
//		void Draw(ImDrawList*, ImVec2, float, NodesState, float, NodeState) const;
//		ConnectorInput(const char*, Meta::Nodes::ConnectorType);
//	};
//	
//	////////////////////////////////////////////////////////////////////////////////
//	
//	struct ConnectorOutput : public Connector
//	{
//		unsigned int connections_ = 0;
//	
//		void Draw(ImDrawList*, ImVec2, float, NodesState, float, NodeState) const;
//		ConnectorOutput(const char*, Meta::Nodes::ConnectorType);
//	};
}