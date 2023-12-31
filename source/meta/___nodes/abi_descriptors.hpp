#pragma once

namespace madX::Meta::Nodes
{
	static const NodeDescMap sNodesDescMap =
	{
		////////////////////////////////////////////////////////////////////////////////

		{
			{ "Camera"},
			{
				NodeType_Camera | NodeFlag_Inherit | NodeFlag_Output,
				//"Camera",
				{
					{ "camera1" },
					{ "camera2" },
					{ "camera3" },
				},
				{},
			},
		},

		////////////////////////////////////////////////////////////////////////////////
		
		{
			{ "Generator"},
			{
				NodeType_Generator | NodeFlag_Component | NodeFlag_Output,
				//"Generator",
				{
					{ "generator1" },
					{ "generator2" },
				},
				{},
			},
		},
		
		////////////////////////////////////////////////////////////////////////////////
		
		{
			{ "Null"},
			{
				NodeType_Geometry | NodeFlag_Inherit | NodeFlag_Output,
				//"Null",
				{},
				{},
			},
		},
		
		///////////////////////////////////////////////////////////////////////////////
		
		{
			{ "Deformer"},
			{
				NodeType_Deformer | NodeFlag_Inherit | NodeFlag_Component,
				//"Deformer",
				{
					{ "deformer1" },
				},
				{},
			},
		},
		
		///////////////////////////////////////////////////////////////////////////////

		{
			{ "Cloner"},
			{
				NodeType_Cloner | NodeFlag_Component | NodeFlag_Inherit | NodeFlag_Output,
				//"Cloner",
				{},
				{},
			},
		},
	
		////////////////////////////////////////////////////////////////////////////////

		{
			{ "Nodes"},
			{
				NodeType_Nodes | NodeFlag_Output,
				//"Nodes",
				{},
				{},
			},
		},

		////////////////////////////////////////////////////////////////////////////////

		{
			{ "Root"},
			{
				NodeType_Root,
				//"Root",
				{},
				{},
			},
		},

		////////////////////////////////////////////////////////////////////////////////
	};

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////

//	static const PropertyDescMap sPropertyDescMap =
//	{
//		{
//			{"Transform"},
//			sTransformProperty
//		}
//	};
}