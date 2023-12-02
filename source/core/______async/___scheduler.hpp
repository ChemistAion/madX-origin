#pragma once

namespace madX::Core
{
	constexpr std::size_t cParallel_PoolSizeWorkers = 4;

	class Scheduler final
	{
		std::atomic_size_t index_;
		std::atomic_bool done_;

		std::array<Executor<128>, cParallel_PoolSizeWorkers> executors_;
		std::array<std::thread, cParallel_PoolSizeWorkers> workers_;

		void Processor(std::size_t index)
		{
			auto& executor = executors_[index];

			for(;;)
			{
				if (executor.TryProcess())
				{
					continue;
				}

				if (done_.load(std::memory_order_relaxed))
				{
					return;
				}
			}
		}
	public:
		Scheduler()
		{
			index_.store(0, std::memory_order_relaxed);
			done_.store(false, std::memory_order_relaxed);

			for (std::size_t index = 0; index < cParallel_PoolSizeWorkers; ++index)
			{
				workers_[index] = std::move(std::thread(&Scheduler::Processor, this, index));
			}
		}

		~Scheduler()
		{
			done_.store(true, std::memory_order_relaxed);

			for (auto& worker : workers_)
			{
				if (worker.joinable())
				{
					worker.join();
				}
			}
		}

		Scheduler(const Scheduler&) = delete;
		Scheduler& operator=(const Scheduler&) = delete;

		template <class Callable, class... Arguments>
		decltype(auto) AddTask(Callable&& callable, Arguments&&... arguments)
		{
			auto[task, future] = Task::CreateTask(std::forward<Callable>(callable), std::forward<Arguments>(arguments)...);	

			auto index = index_.fetch_add(1, std::memory_order_relaxed);

			for (;;)
			{			
				auto& executor = executors_[index++ & (cParallel_PoolSizeWorkers - 1)];

				if (executor.TryPush(task))
				{
					break;
				}
			}

			return future;
		}
	};
}
