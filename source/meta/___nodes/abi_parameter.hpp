#pragma once

namespace madX::Meta::Nodes
{
	typedef Type ParameterFlag;

	enum ParameterFlag_ : ParameterFlag
	{
		//state
		ParameterFlag_Modified			= ParameterFlag(1) << 0,
		ParameterFlag_Warning			= ParameterFlag(1) << 1,
		ParameterFlag_Error				= ParameterFlag(1) << 2,

		ParameterFlag_Disabled			= ParameterFlag(1) << 3,
		ParameterFlag_Locked			= ParameterFlag(1) << 4,

		//const
		ParameterFlag_Input				= ParameterFlag(1) << 5,
		ParameterFlag_Output			= ParameterFlag(1) << 6,
		ParameterFlag_Visible			= ParameterFlag(1) << 7
	};

	////////////////////////////////////////////////////////////////////////////////

	typedef Type ParameterValueType;
	enum ParameterValueType_ : ParameterValueType
	{
		ParameterValueType_Bool			= ParameterValueType(1) << 12,
		ParameterValueType_Int8			= ParameterValueType(1) << 13,
		ParameterValueType_Int16		= ParameterValueType(1) << 14,
		ParameterValueType_Int32		= ParameterValueType(1) << 15,
		ParameterValueType_Int64		= ParameterValueType(1) << 16,
		ParameterValueType_UInt8		= ParameterValueType(1) << 17,
		ParameterValueType_UInt16		= ParameterValueType(1) << 18,
		ParameterValueType_UInt32		= ParameterValueType(1) << 19,
		ParameterValueType_UInt64		= ParameterValueType(1) << 20,
		ParameterValueType_FP8			= ParameterValueType(1) << 21,
		ParameterValueType_FP16			= ParameterValueType(1) << 22,
		ParameterValueType_FP32			= ParameterValueType(1) << 23,
		ParameterValueType_FP64			= ParameterValueType(1) << 24,
		//ParameterValueType_FP128		= ParameterValueType(1) << 25,
		//ParameterValueType_FP256		= ParameterValueType(1) << 26,
		//ParameterValueType_reserved27	= ParameterValueType(1) << 27,
		//ParameterValueType_reserved28	= ParameterValueType(1) << 28,
		//ParameterValueType_reserved29	= ParameterValueType(1) << 29,
		ParameterValueType_RegEx		= ParameterValueType(1) << 30,
		ParameterValueType_String		= ParameterValueType(1) << 31
	};

	////////////////////////////////////////////////////////////////////////////////

	typedef Type ParameterType;
	enum ParameterType_ : ParameterType
	{
		//ParameterType_reserved32     	= ParameterType(1) << 32,
		//ParameterType_reserved33     	= ParameterType(1) << 33,
		//ParameterType_reserved34     	= ParameterType(1) << 34,
		//ParameterType_reserved35     	= ParameterType(1) << 35,
		ParameterType_Scalar	        = ParameterType(1) << 36,
		ParameterType_Vec2           	= ParameterType(1) << 37,
		ParameterType_Vec3           	= ParameterType(1) << 38,
		ParameterType_Vec4	        	= ParameterType(1) << 39,
		ParameterType_Matrix2        	= ParameterType(1) << 40,
		ParameterType_Matrix3        	= ParameterType(1) << 41,
		ParameterType_Matrix4        	= ParameterType(1) << 42
		//ParameterType_reserved43     	= ParameterType(1) << 43,
		//ParameterType_reserved44     	= ParameterType(1) << 44,
		//ParameterType_reserved45     	= ParameterType(1) << 45,
		//ParameterType_reserved46     	= ParameterType(1) << 46,
		//ParameterType_reserved47     	= ParameterType(1) << 47,
		//ParameterType_reserved48     	= ParameterType(1) << 48,
		//ParameterType_reserved49     	= ParameterType(1) << 49,
		//ParameterType_reserved50     	= ParameterType(1) << 50,
		//ParameterType_reserved51     	= ParameterType(1) << 51,
		//ParameterType_reserved52      = ParameterType(1) << 52,
		//ParameterType_reserved53      = ParameterType(1) << 53,
		//ParameterType_reserved54      = ParameterType(1) << 54,
		//ParameterType_reserved55      = ParameterType(1) << 55
	};

//	typedef Type ParameterMark;
//	enum ParameterMark_ : ParameterMark
//	{
//		ParameterMark_0					= 1 << 56,
//		ParameterMark_1					= 1 << 57,
//		ParameterMark_2					= 1 << 58,
//		ParameterMark_3					= 1 << 59,
//		ParameterMark_4					= 1 << 60,
//		ParameterMark_5					= 1 << 61,
//		ParameterMark_6					= 1 << 62
//	};

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////

	//TODO: move it to custom variant-like strucure

	using ParameterValue = Detail::CISTA::variant
		<
		Meta::Null,
		bool,
		std::int8_t,
		std::int16_t,
		std::int32_t,
		std::int64_t,
		std::uint8_t,
		std::uint16_t,
		std::uint32_t,
		std::uint64_t,
		float,
		double
		>;

	struct ParameterDesc
	{
		ParameterType type_;
		const Meta::String name_;
		ParameterValue value_;
		ParameterValue min_;
		ParameterValue max_;
	};

	struct Parameter
	{
		ParameterType type_;
		const char* name_ = nullptr;
		ParameterValue value_;
		ParameterValue min_;
		ParameterValue max_;

		void* data_ = nullptr;
	};
}
