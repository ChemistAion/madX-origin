#pragma once

#include "_benchmarks.hpp"

using namespace madX;

class Template : public ::benchmark::Fixture
{
private:
	HANDLE process_;
protected:
	Template()
	{
		process_ = ::GetCurrentProcess();

		::setlocale(LC_ALL, ""); //TODO: result
		::CoInitializeEx(NULL, COINIT_MULTITHREADED); //TODO: result
		::SymInitialize(process_, NULL, TRUE); //TODO: result

		//TODO: move it to core
		if constexpr (madX::cmadXMode_Debug)
		{
			int debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

			debugFlag |= _CRTDBG_ALLOC_MEM_DF;
			debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
			debugFlag |= _CRTDBG_LEAK_CHECK_DF;

			_CrtSetDbgFlag(debugFlag);

			//auto mode = _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE;
			auto mode = _CRTDBG_MODE_DEBUG;

			_CrtSetReportMode(_CRT_WARN, mode);
			_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);

			_CrtSetReportMode(_CRT_ERROR, mode);
			_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);

			_CrtSetReportMode(_CRT_ASSERT, mode);
			_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
		}
	}

	~Template()
	{
		::CoUninitialize();
		::SymCleanup(process_);
	}

	void SetUp(const ::benchmark::State&) override
	{
	}

	void TearDown(const ::benchmark::State&) override
	{
	}
};

////////////////////////////////////////////////////////////////////////////////