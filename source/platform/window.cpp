#include "window.hpp"

namespace madX::Platform
{
	LRESULT Window::WindowProcedure(HWND handle, UINT message, WPARAM w_param, LPARAM l_param)
	{
		switch (message)
		{
		case WM_CLOSE:
		{
			::DestroyWindow(handle);
			return 0;
		}
		case WM_DESTROY:
		{
			::PostQuitMessage(0);
			return 0;
		}
		case WM_CREATE: 
		{		
			//TODO: init rest of CREATESTRUCT
			auto create_struct = reinterpret_cast<CREATESTRUCT*>(l_param);
			auto window = reinterpret_cast<Window*>(create_struct->lpCreateParams);

			//TODO: error handling...
			::SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)(window));
			return 0;
		}
		[[likely]] default:
		{
			break;
		}
		}

		////////////////////////////////////////////////////////////////////////////////

		const auto user_data = ::GetWindowLongPtr(handle, GWLP_USERDATA);
		const auto window = reinterpret_cast<Window*>(user_data);

		if (window && window->IsExisting() && window->user_pump_)
		{
			LRESULT const result = window->user_pump_(handle, message, w_param, l_param);

			if (result != 0)
			{
				return result;
			}
		}

		////	//TODO: result handling...
		////	if (window && !window->pool_pump_.empty())
		////	{
		////		for (auto& pump : window->pool_pump_)
		////		{
		////			pump(handle, message, w_param, l_param);
		////		}
		////	
		////		//	auto& pool = window->pool_pump_;
		////		//	
		////		//	std::vector<LRESULT> result(pool.size());
		////		//	
		////		//	pool.front()(handle, message, w_param, l_param);
		////		//	
		////		//	result[0] = pool.front()(handle, message, w_param, l_param);
		////		//
		////		//	if (result[0] == 0)
		////		//	{
		////		//		return 0;
		////		//	}
		////	}
	
		return ::DefWindowProc(handle, message, w_param, l_param);	
	}

	Window::Window()
	{
		::SetLastError(ERROR_SUCCESS);

		HICON icon = (HICON)(::LoadImage(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE));

		WNDCLASSEX window_class =
		{
			sizeof(WNDCLASSEX),
			CS_CLASSDC,
			WindowProcedure,
			0,
			0,
			::GetModuleHandle(NULL),
			icon,
			::LoadCursor((HINSTANCE)NULL, IDC_APPSTARTING),
			::CreateHatchBrush(HS_DIAGCROSS, RGB(24, 24, 24)),
			NULL,
			cWindowClass_Name,
			NULL
		};

		atom_ = ::RegisterClassEx(&window_class);
		error_ = ::GetLastError();
	}

	Window::~Window()
	{
		//TODO: error handling...
		::UnregisterClass(cWindowClass_Name, ::GetModuleHandle(NULL));
	}

	void Window::Create()
	{
		//TODO: error handling...
		assert(atom_ != 0);

		handle_ = ::CreateWindowEx
		(
			NULL,
			MAKEINTATOM(atom_),
			cWindow_Name,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,
			NULL,
			::GetModuleHandle(NULL),
			this //TODO: consider this as <Singleton> feature
		);
	
		if (handle_ != NULL)
		{
			HDC hdc = ::GetDC(handle_);
	
			::SetBkColor(hdc, RGB(12, 12, 12));
			::SetBkMode(hdc, OPAQUE);
	
			::ShowWindow(handle_, SW_SHOWNORMAL);
			::UpdateWindow(handle_);

			::SetWindowLongPtr(GetHandle(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)); //TODO: consider this as <Singleton> feature
		}
	}
}