#pragma once

namespace madX::Root::Event
{
	namespace Frame
	{
		struct Update
		{
			const double total_ = 0.0;
			const double delta_ = 0.0;		

			std::uint32_t width_ = 0;
			std::uint32_t height_ = 0;
		};

		struct Begin
		{
		};

		struct Render
		{
		};

		struct End
		{
		};
	};

}