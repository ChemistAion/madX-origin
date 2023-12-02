#pragma once

namespace madX::Meta
{
	class Config final
	{
		const Object object_;

		//TODO: noexcept, make it const -> move it to ctor
		Object Build(const std::filesystem::path& path)
		{
			auto stream = Core::Filesystem::Stream(path);
			return JSON::GetMetaObject(stream);
		}

	public:
		Config() noexcept = default;

		explicit Config(const std::filesystem::path& path) noexcept
		  : object_(Build(path))
		{
		}

		////////////////////////////////////////////////////////////////////////////////

		bool IsEnabled(const char* name) const noexcept;

		//TODO: POD only, Meta::Value
		template <typename Type>
		std::pair<Type, bool> GetValue(const char* name) const noexcept
		{
			if (auto element = object_.find(String(name)); element != object_.end())
			{
				if (const auto result = cista::get_if<Type>(element->second); result != nullptr)
				{
					return { *result, true };
				}
			}

			return { Type(), false };
		}

		std::vector<std::string> GetNamesList(const char* name) const noexcept;
	};
}