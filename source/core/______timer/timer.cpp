#include "_core.hpp"

namespace madX::Core
{
	Timer::Timer()
	{
		paused_ = true;

		auto clock  = std::chrono::steady_clock::now();
		auto time = std::chrono::duration<double>(clock.time_since_epoch()).count();

		start_time_ = time;
		current_time_ = time;
		lastupdate_time_ = time;
		pausestart_time_ = time;
		pausetotal_time_ = 0.0;
	}

	double Timer::GetFlowTime() const
	{
		if (paused_)
		{
			return (pausestart_time_ - pausetotal_time_) - start_time_;
		}
		else
		{
			return (current_time_ - pausetotal_time_) - start_time_;
		}
	}

	double Timer::GetUpdateDelta() const
	{
		auto clock = std::chrono::steady_clock::now();
		auto time = std::chrono::duration<double>(clock.time_since_epoch()).count();

		return time - current_time_;
	}

	void Timer::Pause(bool paused)
	{
		if (paused_ && paused)
		{
			return;
		}	

		if (!paused_ && !paused)
		{
			return;
		}
	
		if (paused)
		{
			pausestart_time_ = current_time_;
			paused_ = true;
		}			
		else
		{
			pausetotal_time_ = (current_time_ - pausestart_time_) + pausetotal_time_;
			paused_ = false;
		}		
	}

	void Timer::Update()
	{
		auto clock = std::chrono::steady_clock::now();
		auto time = std::chrono::duration<double>(clock.time_since_epoch()).count();
		
		lastupdate_time_ = current_time_;
		current_time_ = time;
	}
}