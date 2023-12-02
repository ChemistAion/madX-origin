export module madX.Core.Filesystem;

import stl.core;
import stl.filesystem;
import stl.streaming;

export namespace madX::Core::Filesystem
{
	//TODO: cista me
	namespace Detail
	{
		//TODO: noexcept
		template <typename Symbol = char, typename Traits = std::char_traits<Symbol>, typename Allocator = std::allocator<Symbol>>
		std::basic_string<Symbol, Traits, Allocator> Stream(const std::basic_istream<Symbol, Traits>& in, Allocator allocator = {})
		{
			std::basic_ostringstream<Symbol, Traits, Allocator> stream(std::basic_string<Symbol, Traits, Allocator>(std::move(allocator)));
			stream << in.rdbuf();

			return stream.str();
		}
	}

	class Stream final
	{
		std::string stream_;

		//TODO: noexcept
		std::string Build(const std::filesystem::path& path)
		{
			if (false == std::filesystem::exists(path))
			{
				return {};
			}

			if (false == std::filesystem::is_regular_file(path))
			{
				return {};
			}

			if (std::filesystem::is_empty(path))
			{
				return {};
			}

			////////////////////////////////////////////////////////////////////////////////

			std::ifstream stream(path, std::ifstream::binary);
			
			if (stream.is_open() && stream.good())
			{
				return Detail::Stream(stream);
			}

			return {};
		}

	public:
		Stream() noexcept = default;

		//TODO: noexcept
		explicit Stream(const std::filesystem::path& path)
		  : stream_(Build(path))
		{
		}

		////////////////////////////////////////////////////////////////////////////////

		std::string_view Get() const noexcept
		{
			return stream_;
		}

		std::string& GetStream() noexcept
		{
			return stream_;
		}

		bool IsEmpty() const noexcept
		{
			return stream_.empty();
		}
	};
}