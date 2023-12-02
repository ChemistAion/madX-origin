#pragma once

#include <stdio.h>

namespace madX::Core
{
	constexpr decltype(auto) cConsoleTitle_Name = _T("madX::Console");

	class Console final : public Singleton<Console>
	{
		friend Singleton<Console>;
		Console();
	};
}