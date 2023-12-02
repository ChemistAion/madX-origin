#pragma once

namespace madX::Root::Event
{
	//TODO: move it to frame
	struct Resize
	{
		std::uint32_t width_ = 0;
		std::uint32_t height_ = 0;
	};
}