#pragma once

namespace madX::Core::Filesystem
{
	enum class EntryState
	{
		Unknown = 0,
		Touch = 1,
		Valid,
		Created,
		Modified,
		Removed
	};

	using Element = std::filesystem::directory_entry;

	class Entry final
	{
		EntryState state_ = EntryState::Unknown;

		Element current_ = {};
		Element previous_ = {};
	public:
		Entry() = default;

		explicit Entry(const std::filesystem::path& path) noexcept
		{
			std::error_code error;

			Element element;
			
			if (element.assign(path, error); error)
			{
				return;
			}
			
			if (auto result = element.exists(error); error || false == result)
			{
				return;
			}
			
			state_ = EntryState::Created;
			current_ = previous_ = element;
		}

		Entry(const Entry&) = delete;
		Entry& operator=(const Entry&) = delete;

		Entry(Entry&& other) noexcept = delete;
		Entry& operator=(Entry&& other) noexcept = delete;

		////////////////////////////////////////////////////////////////////////////////

		EntryState Touch() noexcept
		{
			//assert(state_ != EntryState::Unknown);
			assert(state_ != EntryState::Removed);

			std::error_code error;
			current_.refresh(error);

			return error
				? state_ = EntryState::Removed
				: state_ = EntryState::Touch;
		}

		EntryState Update() noexcept
		{		
			assert(state_ == EntryState::Touch);

			auto modified = previous_.last_write_time() != current_.last_write_time();

			//	if (state _== EntryState::Created)
			//	{
			//		assert(false == modified);
			//	
			//		state = EntryState::Valid;
			//		return EntryState::Created;
			//	}

			if (modified)
			{
				previous_ = current_;
				return state_ = EntryState::Modified;
			}
			else
			{
				return state_ = EntryState::Valid;
			}
		}
	};
}