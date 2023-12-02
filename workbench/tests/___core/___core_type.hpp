#pragma once

#include "core/_core.hpp"

namespace Primitives
{	
	template <typename Type>
	struct EmptyTestType
	{
	};

	template <typename Type>
	struct StaticTestType
	{
		static Type test;
	};
}

class CoreType : public Template
{
};