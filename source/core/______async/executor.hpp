#pragma once

#pragma warning(disable : 4324) // structure was padded due to __declspec(align())

namespace madX::Core
{
	template <std::size_t Capacity>
	class Executor final
	{
		struct Cell
		{
			std::atomic_size_t sequence_;
			Task task_; //TODO: check Task size, optimize Cell size
		};

		alignas(std::hardware_destructive_interference_size) Cell* storage_;
		alignas(std::hardware_destructive_interference_size) std::atomic_size_t enqueue_;
		alignas(std::hardware_destructive_interference_size) std::atomic_size_t dequeue_;

	public:
		Executor()
		{
			static_assert((Capacity > 0 && ((Capacity & (Capacity - 1)) == 0)));

			storage_ = new Cell[Capacity];

			for (std::size_t index = 0; index < Capacity; ++index)
			{
				storage_[index].sequence_.store(index, std::memory_order_relaxed);
			}

			enqueue_.store(0, std::memory_order_relaxed);
			dequeue_.store(0, std::memory_order_relaxed);
		}

		~Executor()
		{
			delete[] storage_;
		}

		Executor(const Executor&) = delete;
		Executor& operator=(const Executor&) = delete;

		Executor(Executor&& other) noexcept = default;
		Executor& operator=(Executor&& other) noexcept = default;

		bool TryPush(Task& task)
		{
			Cell* cell;
			auto position = enqueue_.load(std::memory_order_relaxed);

			////////////////////////////////////////////////////////////////////////////////

			for (;;)
			{
				cell = &storage_[position & (Capacity - 1)];

				const auto sequence = cell->sequence_.load(std::memory_order_acquire);

				const auto diff = (std::intptr_t)sequence - (std::intptr_t)position;

				if (diff == 0)
				{
					if (enqueue_.compare_exchange_weak(position, position + 1, std::memory_order_acq_rel))
					{
						break;
					}

					continue;
				}

				if (diff < 0)
				{
					return false;
				}

				position = enqueue_.load(std::memory_order_relaxed);
			}

			////////////////////////////////////////////////////////////////////////////////

			cell->task_ = std::move(task);
			cell->sequence_.store(position + 1, std::memory_order_release);

			return true;
		}

		bool TryProcess()
		{
			const auto position = dequeue_.load(std::memory_order_relaxed);
			auto cell = &storage_[position & (Capacity - 1)];
			const auto sequence = cell->sequence_.load(std::memory_order_acquire);

			if (sequence == position + 1)
			{
				if (false == cell->task_.IsValid())
				{
					dequeue_.fetch_add(1, std::memory_order_relaxed);
					cell->sequence_.store(position + Capacity, std::memory_order_release);

					return true;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (false == cell->task_.IsReady())
				{
					const auto end = enqueue_.load(std::memory_order_relaxed);

					for (auto ready = position + 1; ready < end; ++ready)
					{
						auto victim = &storage_[ready & (Capacity - 1)];

						if (false == victim->task_.IsValid())
						{
							continue;
						}

						if (victim->task_.IsReady())
						{
							victim->task_.Invoke();
						}
					}

					return true;
				}

				////////////////////////////////////////////////////////////////////////////////

				dequeue_.fetch_add(1, std::memory_order_relaxed);

				cell->task_.Invoke();
				cell->sequence_.store(position + Capacity, std::memory_order_release);

				return true;
			}

			////////////////////////////////////////////////////////////////////////////////

			return false;
		}
	};
}
