#pragma once

namespace madX::Meta::Nodes
{
	typedef Type NodeFlag;
	enum NodeFlag_ : NodeFlag
	{
		//state
		NodeFlag_Modified		= NodeFlag(1) << 0,
		NodeFlag_Warning		= NodeFlag(1) << 1,
		NodeFlag_Error			= NodeFlag(1) << 2,

		NodeFlag_Disabled		= NodeFlag(1) << 3,
		NodeFlag_Locked			= NodeFlag(1) << 4,

		//const
		NodeFlag_Component		= NodeFlag(1) << 5,
		NodeFlag_Inherit		= NodeFlag(1) << 6,
		NodeFlag_Output			= NodeFlag(1) << 7
	};

	////////////////////////////////////////////////////////////////////////////////

	typedef Type NodeType;
	enum NodeType_ : NodeType
	{
		NodeType_Animation		= NodeType(1) << 8,
		NodeType_Camera			= NodeType(1) << 9,
		NodeType_Cloner			= NodeType(1) << 10,
		NodeType_Code			= NodeType(1) << 11,
		NodeType_Deformer		= NodeType(1) << 12,
		NodeType_Field			= NodeType(1) << 13,
		NodeType_Generator		= NodeType(1) << 14,
		NodeType_Geometry		= NodeType(1) << 15,
		NodeType_Light			= NodeType(1) << 16,
		NodeType_Logic			= NodeType(1) << 17,
		NodeType_Material		= NodeType(1) << 18,
		NodeType_Math			= NodeType(1) << 19,
		NodeType_Nodes			= NodeType(1) << 20,
		NodeType_Modifier		= NodeType(1) << 21,
		NodeType_Particle		= NodeType(1) << 22,
		NodeType_Physics		= NodeType(1) << 23,
		NodeType_PostFX			= NodeType(1) << 24,
		NodeType_Procedural		= NodeType(1) << 25,
		NodeType_RayTracing		= NodeType(1) << 26,
		NodeType_Root			= NodeType(1) << 27,
		NodeType_Shading		= NodeType(1) << 28,
		NodeType_Sound			= NodeType(1) << 29,
		NodeType_Text			= NodeType(1) << 30,
		NodeType_Video			= NodeType(1) << 31
	};

	////////////////////////////////////////////////////////////////////////////////

	typedef Type NodeGeometry;
	enum NodeGeometry_ : NodeGeometry
	{
		NodeGeometry_Null				= NodeType(1) << 32
	};

	////////////////////////////////////////////////////////////////////////////////

	constexpr Type cNodeTypeNull = 0;

	constexpr Type cNodeABI_FlagCount = 8;
	constexpr Type cNodeABI_FlagBitStart = 0;
	constexpr Type cNodeABI_FlagBitStop = 7;
	constexpr Type cNodeABI_FlagMask = 0xff;

	constexpr Type cNodeABI_TypeCount = 24;
	constexpr Type cNodeABI_TypeBitStart = 8;
	constexpr Type cNodeABI_TypeBitStop = 31;
	constexpr Type cNodeABI_TypeMask = 0xffffff00;

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////

	struct InputDesc
	{
		Meta::String name_;
	};

	struct NodeDesc
	{
		NodeType type_ = cNodeTypeNull;
		//Meta::String name_;
		Meta::Index<InputDesc> inputs_;
		Meta::Vector<PropertyDesc> attributes_;
	};

	using NodeDescMap = Meta::Map<Meta::String, NodeDesc>;
}