#pragma once

namespace madX::Core
{
	namespace Detail
	{
		template <typename Type>
		inline Type AlignUpWithMask(Type value, std::size_t mask)
		{
			return (Type)(((std::size_t)value + mask) & ~mask);
		}

		template <typename Type>
		inline Type AlignDownWithMask(Type value, std::size_t mask)
		{
			return (Type)((std::size_t)value & ~mask);
		}
	}

	//TODO: unsinged types only
	template <typename Type>
	constexpr bool IsSingleBit(Type value)
	{
		return value > 0 && (value & (value - 1)) == 0;
	}

	template <typename Type>
	inline Type AlignUp(Type value, std::size_t alignment)
	{
		assert(IsSingleBit(alignment));
		return Detail::AlignUpWithMask(value, alignment - 1);
	}

	template <typename Type>
	inline Type AlignDown(Type value, std::size_t alignment)
	{
		assert(IsSingleBit(alignment));
		return Detail::AlignDownWithMask(value, alignment - 1);
	}
}