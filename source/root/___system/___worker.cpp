#include "_root.hpp"

namespace madX::Root::System
{
	void Worker::Process(ECS::Registry& registry, Event::Terminate&&)
	{
		//std::lock_guard<std::mutex> lock(lock_);

		for (auto& worker : workers_)
		{
			//TODO: result
			worker.request_stop();
		}
	}

	void Worker::Process(ECS::Registry& registry, Event::Job&& job)
	{
		const auto size = executors_.size();

		for (auto loop = size; loop != 0; --loop)
		{
			auto& executor = executors_[index_++ % size];
			
			if (executor->TryPush(job)) [[likely]]
			{	
				break;
			}
		}

		assert(false == job.IsValid());

		cv_.notify_all();
	}
}