export module madX.Core.Synchron;

import stl.core;
import stl.threading;

export namespace madX::Core
{
	//TODO: use new c++20 stuff
	class Synchron
	{
		int count_ = 0;
		std::mutex lock_;
		std::condition_variable condition_;

	public:
		int Inc()
		{
			std::unique_lock<std::mutex> lock(lock_);

			++count_;

			return count_;
		}

		int Dec()
		{
			std::unique_lock<std::mutex> lock(lock_);

			--count_;

			if (count_ == 0)
			{
				condition_.notify_all();
			}

			return count_;
		}

		int Get()
		{
			std::unique_lock<std::mutex> lock(lock_);

			return count_;
		}

		void Reset()
		{
			std::unique_lock<std::mutex> lock(lock_);

			count_ = 0;
			condition_.notify_all();
		}

		void Wait()
		{
			std::unique_lock<std::mutex> lock(lock_);

			while (count_ != 0)
			{
				condition_.wait(lock);
			}
		}
	};
}