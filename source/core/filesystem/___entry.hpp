#pragma once

namespace madX::Core::Filesystem
{
	enum class EntryState
	{
		Valid = 0,
		Created,
		Modified,
		Unknown
	};

	class Entry final
	{
		std::filesystem::directory_entry entry_ = {};

	public:
		Entry() noexcept = default;

		explicit Entry(const std::filesystem::path& path) noexcept
		{
			std::error_code error;

			std::filesystem::directory_entry element(path, error);

			if (error) [[unlikely]]
			{
				return;
			}

			if (auto exists = element.exists(error); error || !exists) [[unlikely]]
			{
				return;
			}

			entry_ = std::move(element);
		}

		////////////////////////////////////////////////////////////////////////////////

		const auto& GetEntry() const noexcept
		{
			return entry_;
		}

		[[nodiscard]] EntryState Touch() noexcept
		{
			std::error_code error;

			auto previous = entry_.last_write_time(error);

			if (error) [[unlikely]]
			{
				return EntryState::Unknown;
			}

			if (entry_.refresh(error); error) [[unlikely]]
			{
				return EntryState::Unknown;
			}

			if (auto modified = previous != entry_.last_write_time(error); error) [[unlikely]]
			{
				return EntryState::Unknown;
			}
			else if (modified) [[unlikely]]
			{
				if (std::filesystem::last_write_time(entry_, previous, error); error) [[unlikely]]
				{
					return EntryState::Unknown;
				}

				return EntryState::Modified;
			}

			return EntryState::Valid;
		}
	};
}