import stl.core;
import microsoft.windows;
import madX.Core.Singleton;

namespace madX::Platform
{
	constexpr decltype(auto) cWindow_Name = TEXT("madX::Root");
	constexpr decltype(auto) cWindowClass_Name = TEXT("madX::RootClass");

	using MessagePump = std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>;

	class Window final : public Core::Singleton<Window>
	{
	private:
		HWND handle_ = NULL;
		DWORD error_ = ERROR_SUCCESS;
	
		ATOM atom_ = 0;
	
		MessagePump user_pump_;
	
		//bool cursor_clip_ = false;
	
		static LRESULT WindowProcedure(HWND handle, UINT message, WPARAM w_param, LPARAM l_param);
	
		friend Core::Singleton<Window>;
	
		Window();
		~Window();
	public:
		HWND GetHandle() const noexcept
		{
			return handle_;
		}
	
		auto GetWindowWidth() const noexcept
		{
			RECT rect;
			::GetWindowRect(handle_, &rect);
	
			return rect.right - rect.left;
		}
	
		auto GetWindowHeight() const noexcept
		{
			RECT rect;
			::GetWindowRect(handle_, &rect);
	
			return rect.bottom - rect.top;
		}
	
		auto GetClientWidth() const noexcept
		{
			RECT rect;
			::GetClientRect(handle_, &rect);
	
			return rect.right - rect.left;
		}
	
		auto GetClientHeight() const noexcept
		{
			RECT rect;
			::GetClientRect(handle_, &rect);
	
			return rect.bottom - rect.top;
		}
	
		////////////////////////////////////////////////////////////////////////////////
	
		bool IsExisting() const
		{
			if (GetHandle() != NULL && IsWindow(GetHandle()) == FALSE)
			{
				return false;
			}
	
			return true;
		}
	
////	bool IsCursorClip() const noexcept
////	{
////		return cursor_clip_;
////	}
////	
////	RECT GetClientCursorClip() const
////	{
////		RECT rect;
////		SetRectEmpty(&rect);
////	
////		if (IsExisting())
////		{
////			//TODO: handle returns
////			::GetClipCursor(&rect);
////			::MapWindowPoints(HWND_DESKTOP, handle_, (LPPOINT)(&rect), 2);
////			return rect;
////		}
////	
////		return rect;
////	}
////	
////	void SetMousePos(LONG pos_x, LONG pos_y)
////	{
////		if (IsExisting())
////		{
////			POINT point;
////			point.x = pos_x;
////			point.y = pos_y;
////	
////			//TODO: handle returns
////			MapWindowPoints(handle_, HWND_DESKTOP, &point, 1);
////			SetCursorPos(point.x, point.y);
////		}
////	}
////	
////	void SetClipCursor(int32_t min_x, int32_t min_y, int32_t max_x, int32_t max_y)
////	{
////		//TODO: handle returns
////		if (IsExisting())
////		{
////			RECT rect;
////			SetRect(&rect, min_x, min_y, max_x, max_y);
////	
////			MapWindowPoints(handle_, HWND_DESKTOP, (LPPOINT)(&rect), 2);
////			ClipCursor(&rect);
////	
////			cursor_clip_ = true;
////		}
////	}
////	
////	void SetClipCursor()
////	{
////		if (IsExisting())
////		{
////			//TODO: handle returns
////			ClipCursor(NULL);
////			cursor_clip_ = false;
////		}
////	}
////
////	////////////////////////////////////////////////////////////////////////////////
	
		void Create();
	
		void RegisterPump(MessagePump&& pump)
		{
			user_pump_ = std::move(pump);
		}
	};
};