#include "_core.hpp"

namespace madX::Core::Filesystem
{
	Volume::Volume(const std::filesystem::path& path) noexcept
	{
		std::error_code error;

		if (Entry entry(path); entry.GetEntry().is_directory(error) && !error)
		{
			root_ = std::move(entry);
		}
	}

	[[nodiscard]] Elements Volume::Touch() noexcept
	{
		Elements result;
		result.reserve(entries_.size());

		//TODO: for each
		for (auto entries = entries_; auto& [route, entry] : entries)
		{
			const auto state = entry.Touch();

			if (state == EntryState::Valid) [[likely]]
			{
				continue;
			}

			result.emplace_back(state, route, entry.GetEntry().path());

			if (state == EntryState::Unknown)
			{
				entries_.erase(route);
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		//TODO: handle this better way... std::optional?
		if (root_.Touch() == EntryState::Unknown) [[unlikely]]
		{
			return result;
		}

		std::error_code error;

		std::filesystem::recursive_directory_iterator iterator(root_.GetEntry(), std::filesystem::directory_options::skip_permission_denied, error);

		if (error) [[unlikely]]
		{
			return result;
		}

		const auto path = root_.GetEntry().path();

		for (auto& entry : iterator)
		{
			bool entry_type = false;

			entry_type |= entry.is_regular_file();
			entry_type |= entry.is_directory();

			if (false == entry_type)
			{
				continue;
			}

			if (auto route = std::filesystem::relative(entry, path, error); !error)
			{
				const auto& insertion = entries_.try_emplace(route.string(), entry);

				if (insertion.second)
				{
					result.emplace_back(EntryState::Created, route.string(), entry);
				}
			}
		}

		return result;
	}
}