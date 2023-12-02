import stl.core;
import stl.threading;
import stl.streaming;
import microsoft.windows;

namespace madX::Core
{
	class LoggerSink
	{
	public:
		LoggerSink() = default;
		virtual ~LoggerSink() = default;
	
		LoggerSink(const LoggerSink&) = delete;
		LoggerSink& operator=(const LoggerSink&) = delete;
	
		virtual void Write(const char*, int) = 0;
		virtual void Flush()
		{
		}
	};

	class SinkMSVC : public LoggerSink
	{
		virtual void Write(const char* message, int count) override
		{
			count;
			OutputDebugStringA(message);
		}
	};

	class SinkSTD : public LoggerSink
	{
		virtual void Write(const char* message, int count) override
		{
			count;
			std::cout << message;// << std::endl;
		}
	};

//	class SinkImGUI : public LoggerSink
//	{
//		virtual void Write(const char* message, int count) override
//		{
//			count;
//			auto editor_log = Editor::Frontend::Log::GetInstance();
//			editor_log->AddLog(message);
//		}
//	};
}