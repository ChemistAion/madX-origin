#pragma once

namespace madX::Root::Component
{
	class Volume
	{
		std::string name_;
		Core::Filesystem::Volume volume_;
		Core::Future<Core::Filesystem::Elements> future_;
	public:
		Volume() = default;
	
		explicit Volume(const std::string& name)
			: name_(name)
			, volume_(std::filesystem::current_path() / name)
		{
		}

		Core::Filesystem::Elements Touch() noexcept
		{
			//TODO: mutex here?
			return volume_.Touch();
		}

		const auto& GetName() const noexcept
		{
			return name_;
		}

		auto& GetFuture() noexcept
		{
			return future_;
		}

		auto IsValid() const noexcept
		{
			return future_.IsValid();
		}

		auto IsReady() const noexcept
		{
			return future_.IsReady();
		}
	};
}