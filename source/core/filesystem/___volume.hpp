#pragma once

namespace madX::Core::Filesystem
{
	struct Element
	{
		EntryState state_;
		std::string route_;
		std::filesystem::path path_;
	};

	using Elements = std::vector<Element>;

	class Volume final
	{
		Entry root_;
		std::map<std::string, Entry> entries_;

	public:
		Volume() = default;

		explicit Volume(const std::filesystem::path& path) noexcept;

		[[nodiscard]] Elements Touch() noexcept;
	};
}