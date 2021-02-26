/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <thread>
#include <amtrs/amtrs.hpp>
#include <amtrs/window.hpp>
#include <amtrs/.driver/win32-window-window.hpp>
AMTRS_NAMESPACE_BEGIN


size_t window::alives() noexcept
{
	return	window_win32::sWindowCount;
}


void window::message(std::function<handler_func> const& _event)
{
	::MSG		msg;
	while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return;
		}
		auto	ptr = GetWindowLongPtr(msg.hwnd, GWLP_USERDATA);
		if (_event)
		{
			if (ptr)
			{
				if (reinterpret_cast<window_win32*>(ptr)->invoke(_event, msg.hwnd, msg.message, msg.wParam, msg.lParam))
				{
					continue;
				}
			}
		}
		if (msg.message == WM_PAINT && reinterpret_cast<window_win32*>(ptr)->mRepaintCallback)
		{
			reinterpret_cast<window_win32*>(ptr)->mRepaintCallback();
			reinterpret_cast<window_win32*>(ptr)->mRepaintCallback	= {};
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


ref<window> window::create(size_type _size, std::function<handler_func> _event)
{
	ref<window_win32>	w	= new window_win32(std::move(_event));
	w->initialize(nullptr, _size, "LIBAMTRS", "LIBAMTRS");
	return	w;
}


bool window_win32::invoke(std::function<typename super_type::handler_func> const& _handler, HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
{
	switch (_msg)
	{
		case WM_ACTIVATE :
		{
			event_data	e(event_type::activate);
			e.wm.activate	= _wparam != WA_INACTIVE;
			return	_handler(*this, e);
		}

		case WM_CREATE :
		{
			event_data	e(event_type::created);
			return	_handler(*this, e);
		}

		case WM_DESTROY :
		{
			event_data	e(event_type::destroyed);
			return	_handler(*this, e);
		}

		case WM_KEYDOWN :
		case WM_KEYUP :
		{
			keyboard_event	ie;
			ie.key		= vk_to_keycode((int)_wparam);
			ie.state		= _msg == WM_KEYDOWN
						? mouse_event::status::press
						: mouse_event::status::release;
			if (ie.key == key_code::unkown)
			{
				break;
			}
			event_data	e(event_type::input);
			e.wm.input	= &ie;
			return	_handler(*this, e);
		}

		case WM_KILLFOCUS :
		case WM_SETFOCUS :
		{
			event_data	e(event_type::focus);
			e.wm.focus	= _msg == WM_SETFOCUS;
			return	_handler(*this, e);
		}

		case WM_LBUTTONDOWN :
		case WM_LBUTTONUP :
		case WM_MBUTTONDOWN :
		case WM_MBUTTONUP :
		case WM_RBUTTONDOWN :
		case WM_RBUTTONUP :
		case WM_MOUSEMOVE :
		{
			mouse_event	ie;
			switch (_msg)
			{
				case WM_LBUTTONDOWN :
					ie.buttons	= mouse_event::left;
					ie.state	= mouse_event::status::press;
					add_capture();
					break;
					
				case WM_LBUTTONUP :
					ie.buttons	= mouse_event::left;
					ie.state	= mouse_event::status::release;
					dec_capture();
					break;

				case WM_RBUTTONDOWN :
					ie.buttons	= mouse_event::right;
					ie.state	= mouse_event::status::press;
					add_capture();
					break;
					
				case WM_RBUTTONUP :
					ie.buttons	= mouse_event::right;
					ie.state	= mouse_event::status::release;
					dec_capture();
					break;

				case WM_MBUTTONDOWN :
					ie.buttons	= mouse_event::center;
					ie.state	= mouse_event::status::press;
					add_capture();
					break;

				case WM_MBUTTONUP :
					ie.buttons	= mouse_event::center;
					ie.state	= mouse_event::status::release;
					dec_capture();
					break;

				case WM_MOUSEMOVE :
					ie.buttons	= (_wparam & MK_LBUTTON ? mouse_event::left   : 0)
								| (_wparam & MK_RBUTTON ? mouse_event::right  : 0)
								| (_wparam & MK_RBUTTON ? mouse_event::center : 0)
					;
					ie.state	= ie.buttons ? mouse_event::status::drag : mouse_event::status::move;
					break;
			}
			ie.point	= mouse_event::point_type(GET_X_LPARAM(_lparam), GET_Y_LPARAM(_lparam));

			event_data	e(event_type::input);
			e.wm.input	= &ie;
			return	_handler(*this, e);
		}

		case WM_MOUSEWHEEL :
		{
			POINT	pt	= {GET_X_LPARAM(_lparam), GET_Y_LPARAM(_lparam)};
			ScreenToClient(_hwnd, &pt);
			short				z_delta	= GET_WHEEL_DELTA_WPARAM(_wparam);
			float				z_roll	= (float)z_delta / (float)WHEEL_DELTA;
			mouse_wheel_event	ie;
			ie.point.x			= pt.x;
			ie.point.y			= pt.y;
			ie.axis				= mouse_wheel_event::axis_y;
			ie.delta			= z_roll;

			event_data	e(event_type::input);
			e.wm.input	= &ie;
			return	_handler(*this, e);
		}

		case WM_PAINT :
		{
			HDC			hdc;
			PAINTSTRUCT	ps;
			hdc		= BeginPaint(_hwnd, &ps);
			event_data	e(event_type::repaint);
			e.wm.repaint	= rect<int>
			{
				{(int)ps.rcPaint.left,
				(int)ps.rcPaint.top},
				{(int)(ps.rcPaint.right-ps.rcPaint.left),
				(int)(ps.rcPaint.bottom-ps.rcPaint.top)}
			};
			bool	ret	= _handler(*this, e);
			EndPaint(_hwnd, &ps);
			return	ret;
		}

		case WM_SIZE :
		{
			event_data	e(event_type::sizechanged);
			e.wm.sizechanged	= size();
			return	_handler(*this, e);
		}
	}
	return	false;
}


void window_win32::initialize(HWND _parent, size_type _size, LPCSTR _classname, LPCSTR _caption)
{
	// ウィンドウクラスの登録
	WNDCLASSEX	wcls;
	wcls.cbSize			= sizeof(wcls);
	wcls.style			= CS_HREDRAW | CS_VREDRAW;
	wcls.cbClsExtra		= 0;
	wcls.cbWndExtra		= sizeof(window_win32*);
	wcls.hInstance		= GetModuleHandle(nullptr);
	wcls.hIcon			= LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION);
	wcls.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcls.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcls.lpszMenuName	= nullptr;
	wcls.lpszClassName	= _classname;
	wcls.hIconSm		= LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION);
	wcls.lpfnWndProc	= [](HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) -> LRESULT
	{
		LONG_PTR		ptr		= GetWindowLongPtr(_hwnd, GWLP_USERDATA);
		switch (_msg)
		{
			case WM_NCCREATE :
			{
				// ウィンドウが作成されたので自身を登録
				CREATESTRUCT*	cs		= reinterpret_cast<CREATESTRUCT*>(_lparam);
				window_win32*	thiz	= reinterpret_cast<window_win32*>(cs->lpCreateParams);
				SetWindowLongPtr(_hwnd, GWLP_USERDATA, (LONG_PTR)thiz);
				thiz->mDC	= GetDC(_hwnd);
				ptr			= reinterpret_cast<LONG_PTR>(thiz);
				break;
			}

			case WM_CREATE :
			{
				++sWindowCount;
				break;
			}

			case WM_NCDESTROY :
			{
				--sWindowCount;
				break;
			}

			case WM_WINDOWPOSCHANGING :
			{
				// ウィンドウ枠のサイズを計算する
				RECT	cr, wr;
				GetWindowRect(_hwnd, &wr);
				GetClientRect(_hwnd, &cr);
				int		border_w	= (wr.right  - wr.left) - (cr.right  - cr.left);
				int		border_h	= (wr.bottom - wr.top ) - (cr.bottom - cr.top);

				// クライアント領域を偶数になるように調整する
				WINDOWPOS&	wp	= *reinterpret_cast<WINDOWPOS*>(_lparam);
				wp.cx	= (((wp.cx - border_w) + 1) & ~1) + border_w;
				wp.cy	= (((wp.cy - border_h) + 1) & ~1) + border_h;
				break;
			}
		}
		if (ptr)
		{
			auto	thiz	= reinterpret_cast<window_win32*>(ptr);
			if (thiz->mHandler && thiz->invoke(thiz->mHandler, _hwnd, _msg, _wparam, _lparam))
			{
				return	true;
			}
		}
		return	DefWindowProc(_hwnd, _msg, _wparam, _lparam);
	}; // wcls.lpfnWndProc


	if (!RegisterClassEx(&wcls))
	{
		throw	std::system_error(os::win32::make_last_error_code(), "RegisterClassEx()");
	}

	DWORD	dwStyle		= WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
	DWORD	dwExStyle	= WS_EX_APPWINDOW;
	if (_parent)
	{
		dwStyle		= WS_CHILDWINDOW;
		dwExStyle	= WS_EX_TRANSPARENT;
	}


	// ウィンドウの大きさを計算する
	RECT	rect;
	rect.left	= 0;
	rect.top	= 0;
	rect.right	= (DWORD)_size.width;
	rect.bottom	= (DWORD)_size.height;
	AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);
	mWnd	= CreateWindowEx(
				dwExStyle,
				wcls.lpszClassName,
				_caption,
				dwStyle,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				rect.right - rect.left,
				rect.bottom - rect.top,
				_parent,
				nullptr,
				GetModuleHandle(nullptr),
				this
			);
	if (!mWnd)
	{
		throw	std::system_error(os::win32::make_last_error_code(), "CreateWindowEx()");
	}
	ShowWindow(mWnd, TRUE);
}



key_code window_win32::vk_to_keycode(int _vk)
{
	switch (_vk)
	{
		case VK_BACK :			return	key_code::backspace_key;
		case VK_TAB :			return	key_code::tab_key;
		case VK_CLEAR :			return	key_code::clear_key;
		case VK_RETURN  :		return	key_code::return_key;
		case VK_SHIFT :			return	key_code::shift_key;
		case VK_CONTROL :		return	key_code::control_key;
		case VK_MENU :			return	key_code::menu_key;
		case VK_PAUSE :			return	key_code::pause_key;
		case VK_CAPITAL :		return	key_code::capital_key;

		// IME
		case VK_KANA :			return	key_code::kana_key;
		//case VK_HANGEUL :		return	key_code::hangul_key;
		case VK_JUNJA :			return	key_code::junja_key;
		case VK_FINAL :			return	key_code::final_key;
		case VK_HANJA :			return	key_code::hanja_key;
		case VK_ESCAPE :		return	key_code::escape_key;
		case VK_CONVERT :		return	key_code::convert_key;
		case VK_NONCONVERT :	return	key_code::nonconvert_key;
		case VK_ACCEPT :		return	key_code::accept_key;
		case VK_MODECHANGE :	return	key_code::modechange_key;

		case VK_SPACE :			return	key_code::space_key;
		case VK_NEXT :			return	key_code::next_key;
		case VK_END :			return	key_code::end_key;
		case VK_HOME :			return	key_code::home_key;
		case VK_LEFT :			return	key_code::left_key;
		case VK_UP :			return	key_code::up_key;
		case VK_RIGHT :			return	key_code::right_key;
		case VK_DOWN :			return	key_code::down_key;


		default:				return	key_code::unkown;
	}
}


AMTRS_NAMESPACE_END
