#include "sink.hpp"

import madX.Core.Singleton;

namespace madX::Core
{
	static const char* cLogger_LevelName[] = { "T", "D", "I", "W", "E", "C", "X" };

	inline constexpr int cLogger_BufferSize = 256;

	class Logger final : public Singleton<Logger>
	{
	public:
		enum class Level : std::uint8_t
		{
			Trace = 0,
			Debug = 1,
			Info = 2,
			Warning = 3,
			Error = 4,
			Critical = 5,
			Off = 6
		};
	private:
		std::atomic<Level> level_;
		std::atomic<Level> flush_;

		const std::chrono::time_point<std::chrono::high_resolution_clock> time_;

		std::vector<std::unique_ptr<LoggerSink>> sinks_;

		friend Singleton<Logger>;
		Logger();

		void Log(const Level level, const std::string& message) noexcept;


	public:
		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;
		
		template <class... Arguments>
		void Trace(Arguments&&... arguments) noexcept
		{
			Log(Level::Trace, std::forward<Arguments>(arguments)...);
		}
		
		template <class... Arguments>
		void Debug(Arguments&&... arguments) noexcept
		{
			Log(Level::Debug, std::forward<Arguments>(arguments)...);
		}
		
		template <class... Arguments>
		void Info(Arguments&&... arguments) noexcept
		{
			Log(Level::Info, std::forward<Arguments>(arguments)...);
		}
		
		template <class... Arguments>
		void Warning(Arguments&&... arguments) noexcept
		{
			Log(Level::Warning, std::forward<Arguments>(arguments)...);
		}
		
		template <class... Arguments>
		void Error(Arguments&&... arguments) noexcept
		{
			Log(Level::Error, std::forward<Arguments>(arguments)...);
		}
		
		template <class... Arguments>
		void Critical(Arguments&&... arguments) noexcept
		{
			Log(Level::Critical, std::forward<Arguments>(arguments)...);
		}

		void SetLogLevel(Level level) noexcept
		{
			level_.store(level);
		}
		
		void SetFlushLevel(Level level) noexcept
		{
			flush_.store(level);
		}

		void RegisterSink(std::unique_ptr<LoggerSink> sink) noexcept;
	};
}