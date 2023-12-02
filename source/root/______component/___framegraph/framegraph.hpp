#pragma once

#include "engine/renderer.hpp"

namespace madX::Root::Component
{
	class FrameGraph
	{
	public:
		std::vector<Root::Component::PassBase> pass_;
	};
}