#pragma once

namespace madX::Meta::glTF
{
	struct Buffer
	{
		std::optional<std::string> uri_;
		Size size_;

		Data data_;

		Name name_;
		Extras extras_;
	};

	using Buffers = std::vector<Buffer>;
	Buffers GetBuffers(const simdjson::dom::element& element);
}