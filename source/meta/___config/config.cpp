#include "_meta.hpp"

namespace madX::Meta
{
	bool Config::IsEnabled(const char* name) const noexcept
	{
		if (auto element = object_.find(String(name)); element != object_.end())
		{
			if (const auto result = cista::get_if<bool>(element->second); result != nullptr)
			{
				return true;
			}
		}

		return false;
	}

	std::vector<std::string> Config::GetNamesList(const char* name) const noexcept
	{
		if (auto element = object_.find(String(name)); element != object_.end())
		{
			if (const auto result = cista::get_if<MetaArray>(element->second); result != nullptr)
			{
				std::vector<std::string> names;

				for (const auto& element : *result->get())
				{
					if (const auto name = cista::get_if<String>(element); name != nullptr)
					{
						names.emplace_back(name->str());
					}
				}

				return names;
			}
		}

		return {};
	}
}