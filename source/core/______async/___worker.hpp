#pragma once

namespace madX::Core
{
	class Worker final
	{
		std::jthread worker_;
	public:
		Worker() = default;

		template <class Callable, class... Arguments>
		explicit Worker(Callable&& callable, Arguments&&... arguments)
			: worker_(std::forward<Callable>(callable), std::forward<Arguments>(arguments)...)
		{
		}

		Worker(const Worker&) = delete;
		Worker& operator=(const Worker&) = delete;

		Worker(Worker&& other) noexcept = default;
		Worker& operator=(Worker&& other) noexcept = default;

		////////////////////////////////////////////////////////////////////////////////

		auto& Get() noexcept
		{
			return worker_;
		}
	};
}