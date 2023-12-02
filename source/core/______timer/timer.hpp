#pragma once

namespace madX::Core
{
	class Timer final
	{
		double start_time_;
		double current_time_;
		double lastupdate_time_;
		double pausestart_time_;
		double pausetotal_time_;

		bool paused_;
	public:
		Timer();

		////////////////////////////////////////////////////////////////////////////////

		bool IsPaused() const
		{
			return paused_;
		}

		double GetTotalTime() const
		{
			return current_time_ - start_time_;
		}

		double GetUpdateTime() const
		{
			return current_time_ - lastupdate_time_;
		}

		////////////////////////////////////////////////////////////////////////////////

		double GetFlowTime() const;
		double GetUpdateDelta() const;

		void Pause(bool paused);
		void Update();
	};
}

