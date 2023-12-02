#pragma once

namespace madX::Meta::glTF
{
	//unique_ptr + size
	using Data = std::vector<std::byte>;

	////////////////////////////////////////////////////////////////////////////////

	struct glb_header
	{
		std::uint32_t magic_;
		std::uint32_t version_;
		std::uint32_t length_;
	};

	struct glb_chunk
	{
		std::uint32_t length_;
		std::uint32_t type_;
	};

	Data GetData(const std::filesystem::path& path, const std::string& uri);
}