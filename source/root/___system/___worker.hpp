#pragma once

namespace madX::Root::System
{
	constexpr int cSystem_WorkerTasksPoolSize = 128;
	constexpr int cSystem_DefaultWorkersCount = 2;

	class Worker final : public ECS::System<Event::Terminate, Event::Job>
	{
		std::condition_variable_any cv_;
		std::atomic_size_t index_ = 0;

		//std::mutex lock_;

		using Executor = Core::Executor<cSystem_WorkerTasksPoolSize>;

		std::vector<std::jthread> workers_;
		std::vector<std::unique_ptr<Executor>> executors_;

		void Processor(std::stop_token token, Executor* executor)
		{
			std::mutex cv_lock;

			while (false == token.stop_requested())
			{
				std::unique_lock<std::mutex> lock(cv_lock);

				auto result = cv_.wait_for
				(
					lock,
					token,
					100ms,
					[this, &executor] { return executor->TryProcess(); }
				);

				if (false == result)
				{
					std::this_thread::yield();
				}
			}
		}

	public:
		explicit Worker(int count = cSystem_DefaultWorkersCount)
		{
			for (int index = 0; index < count; ++index)
			{
				executors_.emplace_back(std::make_unique<Executor>());

				auto worker = std::jthread
				(
					std::bind_front(&Worker::Processor, this),
					executors_.back().get()
				);

				workers_.emplace_back(std::move(worker));
			}

			return;
		}

		~Worker()
		{
			//TODO: checks...
		}

		////////////////////////////////////////////////////////////////////////////////

		void Process(ECS::Registry& registry, Event::Terminate&&) override;
		void Process(ECS::Registry& registry, Event::Job&&) override;
	};
}