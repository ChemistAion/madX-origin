#pragma once

namespace madX::Core
{
	namespace Detail
	{
		template <typename Type>
		using HasFutureMark = decltype(std::declval<Type>().MarkAsFuture());

		template <typename Type>
		using IsFutureImpl = IsDetected<HasFutureMark, Type>;

		////////////////////////////////////////////////////////////////////////////////

		template <typename Type>
		struct DecayFutureImpl
		{
			using type = Type;
		};

		template <template <typename> class Class, typename Type>
		struct DecayFutureImpl<Class<Type>>
		{
			using type = Type;
		};

		template <template <typename> class Class, typename Type>
		struct DecayFutureImpl<Class<Type>&>
		{
			using type = Type;
		};

		template <template <typename> class Class, typename Type>
		struct DecayFutureImpl<const Class<Type>&>
		{
			using type = Type;
		};
	}

	////////////////////////////////////////////////////////////////////////////////

	template <typename Type>
	constexpr bool IsFuture = Detail::IsFutureImpl<std::decay_t<Type>>::value;

	template <typename Type>
	using DecayFutureType = typename Detail::DecayFutureImpl<Type>::type;

	template <typename Type>
	using DecayIfFuture = std::conditional_t<IsFuture<Type>, DecayFutureType<Type>, Type>;

	////////////////////////////////////////////////////////////////////////////////

	template <typename Type>
	class Future final
	{
	private:
		std::shared_future<Type> future_;

		constexpr void MarkAsFuture() const noexcept
		{
		}

	public:
		Future() noexcept = default;

		explicit Future(std::shared_future<Type>&& future) noexcept
		{
			future_ = std::move(future);
		}

		[[nodiscard]] bool IsValid() const noexcept
		{
			return future_.valid();
		}

		[[nodiscard]] bool IsReady() const
		{
			if (IsValid())
			{
				return future_.wait_for(std::chrono::steady_clock::duration::zero()) == std::future_status::ready;
			}

			return false;
		}

		[[nodiscard]] decltype(auto) Get() const
		{
			return future_.get();
		}

		//using type_ = Type;
	};
}
