#include "logger.hpp"

namespace madX::Core
{
	Logger::Logger()
		: time_(std::chrono::steady_clock::now())
		, level_(Level::Trace)
		, flush_(Level::Info)
	{
	}

	void Logger::Log(const Level level, const std::string& message) noexcept
	{
		if (level < level_.load(std::memory_order_relaxed))
		{
			return;
		}

		const auto time = std::chrono::duration<float>(std::chrono::steady_clock::now() - time_).count();

		char buffer[cLogger_BufferSize];

		auto result = std::snprintf
		(
			buffer,
			cLogger_BufferSize,
			"%s %12.7f | %s\n",
			cLogger_LevelName[(int)level],
			time,
			message.c_str()
		);

		if (result < 0)
		{
			return;
		}

//		auto flush = level < flush_.load(std::memory_order_relaxed) ? false : true;
		auto length = result < cLogger_BufferSize ? result : cLogger_BufferSize - 1;

		for (auto& sink : sinks_)
		{
			sink->Write(buffer, length);
		}
	}

	void Logger::RegisterSink(std::unique_ptr<LoggerSink> sink) noexcept
	{
		//TODO: mutex
		sinks_.push_back(std::move(sink));
	}
}