#pragma once

namespace madX::Meta::glTF
{
	struct Accessor
	{
		enum class ComponentType : Type
		{
			None = 0,
			Byte = 5120, //8b
			UnsignedByte = 5121, //8b
			Short = 5122, //16b
			UnsignedShort = 5123, //16b
			//Integer = 5124, //32b
			UnsignedInteger = 5125, //32b
			Float = 5126 //32b
		};

		//TODO: use DirectX
		enum class ElementType : Type
		{
			None,
			Scalar, //1
			Vec2, //2
			Vec3, //3
			Vec4, //4
			Mat2, //4
			Mat3, //9
			Mat4 //16
		};

		std::optional<Index> view_;
		Offset offset_ = 0;
		Count count_ = 0;
		
		//std::pair<ComponentType, Size> component_ = { ComponentType::None, 0 };
		//std::pair<ElementType, Size> element_ = { ElementType::None, 0 };
		Size component_ = 0;
		Size element_ = 0;

		//TODO: template it, could be integral
		//TODO: same size min and max
		std::vector<Float> max_;
		std::vector<Float> min_;

		Name name_;
		Extras extras_;
	};

	using Accessors = std::vector<Accessor>;
	Accessors GetAccessors(const simdjson::dom::element& element);
}