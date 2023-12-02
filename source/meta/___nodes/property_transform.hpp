#pragma once

namespace madX::Meta::Nodes
{
	static ParameterDesc sTransformParameter_PositionX = 
	{
		ParameterValueType_FP32 | ParameterType_Scalar,
		"Position X",
		ParameterValue((float)0.0f),
		ParameterValue((float)-10.0f),
		ParameterValue((float)+10.0f)
	};

	static ParameterDesc sTransformParameter_PositionY =
	{
		ParameterValueType_FP32 | ParameterType_Scalar,
		"Position Y",
		ParameterValue((float)0.0f),
		ParameterValue((float)-10.0f),
		ParameterValue((float)+10.0f)
	};

	static ParameterDesc sTransformParameter_PositionZ =
	{
		ParameterValueType_FP32 | ParameterType_Scalar,
		"Position Z",
		ParameterValue((float)0.0f),
		ParameterValue((float)-10.0f),
		ParameterValue((float)+10.0f)
	};

	////////////////////////////////////////////////////////////////////////////////

	static ParameterDesc sTransformParameter_RotationX =
	{
		ParameterValueType_FP32 | ParameterType_Scalar,
		"Rotation Pich",
		ParameterValue((float)0.0f),
		ParameterValue((float)-180.0f),
		ParameterValue((float)+180.0f)
	};

	static ParameterDesc sTransformParameter_RotationY =
	{
		ParameterValueType_FP32 | ParameterType_Scalar,
		"Rotation Heading",
		ParameterValue((float)0.0f),
		ParameterValue((float)-180.0f),
		ParameterValue((float)+180.0f)
	};

	static ParameterDesc sTransformParameter_RotationZ =
	{
		ParameterValueType_FP32 | ParameterType_Scalar,
		"Rotation Bank",
		ParameterValue((float)0.0f),
		ParameterValue((float)-180.0f),
		ParameterValue((float)+180.0f)
	};

	////////////////////////////////////////////////////////////////////////////////

	static ParameterDesc sTransformParameter_ScaleX =
	{
		ParameterValueType_FP32 | ParameterType_Scalar,
		"Scale X",
		ParameterValue((float)0.0f),
		ParameterValue((float)0.0f),
		ParameterValue((float)+1.0f)
	};

	static ParameterDesc sTransformParameter_ScaleY =
	{
		ParameterValueType_FP32 | ParameterType_Scalar,
		"Scale Y",
		ParameterValue((float)0.0f),
		ParameterValue((float)0.0f),
		ParameterValue((float)+1.0f)
	};

	static ParameterDesc sTransformParameter_ScaleZ =
	{
		ParameterValueType_FP32 | ParameterType_Scalar,
		"Scale Z",
		ParameterValue((float)0.0f),
		ParameterValue((float)0.0f),
		ParameterValue((float)+1.0f)
	};

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////

	static PropertyDesc sTransformProperty =
	{
		"Transform",
		{
			sTransformParameter_PositionX,
			sTransformParameter_PositionY,
			sTransformParameter_PositionZ,
			sTransformParameter_RotationX,
			sTransformParameter_RotationY,
			sTransformParameter_RotationZ,
			sTransformParameter_ScaleX,
			sTransformParameter_ScaleY,
			sTransformParameter_ScaleZ
		}
	};
}


