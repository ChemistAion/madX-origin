export module madX.Backend.Draw;

import stl.core;

export namespace madX::Backend
{
	struct DrawAttributes
	{
		std::uint32_t vertices_ = 0;
		std::uint32_t instances_ = 1;

		std::uint32_t first_vertex_index_ = 0;
		std::uint32_t first_instance_index_ = 0;

		DrawAttributes() noexcept = default;
	};

	struct DrawIndexedAttributes
	{
		std::uint32_t indices_ = 0;
		std::uint32_t instances_ = 1;

		std::uint32_t base_vertex_ = 0;

		std::uint32_t first_index_index_ = 0;
		std::uint32_t first_instance_index_ = 0;

		DrawIndexedAttributes() noexcept = default;
	};
}