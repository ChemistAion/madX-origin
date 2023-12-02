#include "_core.hpp"

namespace madX::Core
{
	Console::Console()
	{
		if (FALSE == ::AttachConsole(ATTACH_PARENT_PROCESS))
		{
			if (FALSE == ::AllocConsole())
			{
				throw;
			}
		}

		::SetConsoleTitle(cConsoleTitle_Name);

		////////////////////////////////////////////////////////////////////////////////

		FILE* f_stdout = NULL;
		::freopen_s(&f_stdout, "CONOUT$", "w", stdout);

		FILE* f_stderr = NULL;
		::freopen_s(&f_stderr, "CONOUT$", "w", stderr);

		FILE* f_stdin = NULL;
		::freopen_s(&f_stdin, "CONIN$", "r", stdin);

		HANDLE handle_stdout = ::CreateFile(_T("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		SetStdHandle(STD_OUTPUT_HANDLE, handle_stdout);
		SetStdHandle(STD_ERROR_HANDLE, handle_stdout);

		HANDLE handle_stdin = ::CreateFile(_T("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		SetStdHandle(STD_INPUT_HANDLE, handle_stdin);

		std::clog.clear();
		std::cout.clear();
		std::cerr.clear();

		std::wclog.clear();
		std::wcout.clear();
		std::wcerr.clear();

		std::cin.clear();
		std::wcin.clear();
	}
}