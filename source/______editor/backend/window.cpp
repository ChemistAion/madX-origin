#include "_editor.hpp"

#include "window.hpp"
#include "resources.hpp"

#include "backend.hpp"

//https://stackoverflow.com/questions/63096226/directx-resize-shows-win32-background-at-edges
//https://stackoverflow.com/questions/65929978/wm-paint-and-dx12
//https://github.com/walbourn/directx-vs-templates/blob/master/d3d12backend_win32/Main.cpp

namespace madX::Editor
{
	LRESULT Window::WindowProcedure(HWND handle, UINT message, WPARAM w_param, LPARAM l_param)
	{
		auto backend = reinterpret_cast<Backend*>(::GetWindowLongPtr(handle, GWLP_USERDATA));

		if (backend)
		{
			//std::lock_guard<std::mutex> guard(backend->lock_);
			backend->queue_.emplace(handle, message, w_param, l_param);
		}

		switch (message)
		{
		case WM_PAINT:
		{
			RECT rect;
			GetClientRect(handle, &rect);

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(handle, &ps);

			HBRUSH brush = CreateHatchBrush(HS_DIAGCROSS, RGB(24, 24, 24));
			SetBkColor(hdc, RGB(12, 12, 12));
			SetBkMode(hdc, OPAQUE);
			SelectObject(hdc, brush);
			FillRect(hdc, &rect, brush);
			DeleteObject(brush);

			EndPaint(handle, &ps);

			break;
		}

		case WM_GETMINMAXINFO:
		{
			if (l_param)
			{
				auto info = reinterpret_cast<MINMAXINFO*>(l_param);
				info->ptMinTrackSize.x = 320;
				info->ptMinTrackSize.y = 200;
			}

			break;
		}

		case WM_CLOSE:
		{
			auto logger = Core::Logger::GetInstance();
			logger->Info("Editor stop requested");

			//	if (backend)
			//	{
			//		backend->worker_.join();
			//	}

			DestroyWindow(handle);
			break;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}

		case WM_MENUCHAR:
		{
			return MAKELRESULT(0, MNC_CLOSE);
		}
		default:
		{
			break;
		}
		}

		return DefWindowProc(handle, message, w_param, l_param);
	}

	Window::Window(LPCWSTR class_name, LPCWSTR window_name)
	{
		assert(class_name);
		assert(window_name);

		::SetLastError(ERROR_SUCCESS);

		WNDCLASSEX window_class =
		{
			sizeof(WNDCLASSEX),
			CS_HREDRAW | CS_VREDRAW,
			WindowProcedure,
			0,
			0,
			::GetModuleHandle(NULL),
			(HICON)(::LoadImage(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR)),
			::LoadCursor((HINSTANCE)NULL, IDC_APPSTARTING),
			::CreateHatchBrush(HS_DIAGCROSS, RGB(24, 24, 24)),
			NULL,
			class_name,
			NULL
		};

		atom_ = ::RegisterClassEx(&window_class);
		error_ = ::GetLastError();

		////////////////////////////////////////////////////////////////////////////////

		assert(atom_ != 0);

		handle_ = ::CreateWindowEx
		(
			//WS_EX_NOREDIRECTIONBITMAP,
			NULL,
			MAKEINTATOM(atom_),
			window_name,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,
			NULL,
			::GetModuleHandle(NULL),
			NULL
		);

		if (handle_ != NULL)
		{
			//	HDC hdc = ::GetDC(handle_);
			//	
			//	::SetBkColor(hdc, RGB(12, 12, 12));
			//	::SetBkMode(hdc, OPAQUE);
		
			::ShowWindow(handle_, SW_SHOWMAXIMIZED);
			//::ShowWindow(handle_, SW_SHOWNORMAL);
			::UpdateWindow(handle_);
		}
	}

	Window::~Window()
	{
		//TODO: error handling...
		assert(atom_ != 0);
		::UnregisterClass(MAKEINTATOM(atom_), ::GetModuleHandle(NULL));
	}
}