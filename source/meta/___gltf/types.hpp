#pragma once

namespace madX::Meta::glTF
{
	using Bool = bool;

	using Byte = char;
	using UnsignedByte = unsigned char;
	using Short = short;
	using UnsignedShort = unsigned short;
	//using Integer = int;
	using UnsignedInteger = unsigned int;
	using Float = float;

	static_assert(sizeof(Byte) == 1);
	static_assert(sizeof(UnsignedByte) == 1);
	static_assert(sizeof(Short) == 2);
	static_assert(sizeof(UnsignedShort) == 2);
	//static_assert(sizeof(Integer) == 4);
	static_assert(sizeof(UnsignedInteger) == 4);
	static_assert(sizeof(Float) == 4);

	////////////////////////////////////////////////////////////////////////////////

	using Index = UnsignedInteger;
	using Offset = UnsignedInteger;
	using Count = UnsignedInteger;
	using Type = UnsignedInteger;
	using Size = UnsignedInteger;
	using Coord = UnsignedInteger;

	constexpr Index InvalidIndex = std::numeric_limits<Index>::max();
}