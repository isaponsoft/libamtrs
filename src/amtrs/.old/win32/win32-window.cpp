/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/window.hpp>
#include <amtrs/.driver/win32/win32-window-window.hpp>
#include <amtrs/.driver/win32/win32api/win32api-uxtheme.hpp>

AMTRS_NAMESPACE_BEGIN

ref<window> window::create(std::string _title, size_type _size, std::function<handler_func> _proc)
{
	ref<window_win32>	w	= new window_win32(std::move(_proc));
	w->initialize(NULL, _size, "amtrs::win32::driver", _title.c_str());
	return	w;
}

void window_win32::initialize(HWND _parent, size_type _size, LPCSTR _classname, LPCSTR _caption)
{
	os::win32::init_uxtheme();

	// ウィンドウクラスの登録
	WNDCLASSEX	wcls;
	wcls.cbSize			= sizeof(wcls);
	wcls.style			= CS_HREDRAW | CS_VREDRAW;
	wcls.cbClsExtra		= 0;
	wcls.cbWndExtra		= sizeof(window_win32*);
	wcls.hInstance		= GetModuleHandle(nullptr);
	wcls.hIcon			= LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION);
	wcls.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcls.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcls.lpszMenuName	= nullptr;
	wcls.lpszClassName	= _classname;
	wcls.hIconSm		= LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION);
	wcls.lpfnWndProc	= [](HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) -> LRESULT
	{
		LONG_PTR		ptr		= GetWindowLongPtr(_hwnd, GWLP_USERDATA);
		if (_msg == WM_NCCREATE)
		{
			// ウィンドウが作成されたので自身を登録
			// Sample => https://msdn.microsoft.com/ja-jp/library/windows/desktop/ff381400(v=vs.85).aspx
			CREATESTRUCT*	cs		= reinterpret_cast<CREATESTRUCT*>(_lparam);
			window_win32*	thiz	= reinterpret_cast<window_win32*>(cs->lpCreateParams);
			SetWindowLongPtr(_hwnd, GWLP_USERDATA, (LONG_PTR)thiz);
			thiz->mDC	= GetDC(_hwnd);
			ptr	= (LONG_PTR)thiz;

			os::win32::allow_theme(_hwnd, APPMODE_ALLOWDARK);
			SetWindowTheme(_hwnd, L"Explorer", NULL);
		}
		if (ptr)
		{
			return	reinterpret_cast<window_win32*>(ptr)->wndproc(_hwnd, _msg, _wparam, _lparam);
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




LRESULT window_win32::wndproc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
{
	switch (_msg)
	{
		case WM_CREATE :
		{
			mWnd	= _hwnd;
			fire_created();
			break;
		}

		case WM_NCDESTROY :
		{
			// ウィンドウが破棄されたので自身を削除
			fire_destroyed();
			SetWindowLongPtr(_hwnd, GWLP_USERDATA, (LONG_PTR)nullptr);
			PostQuitMessage(0);
			break;
		}

		case WM_KEYDOWN :
		case WM_KEYUP :
		{
			keyboard_event	e;
			e.key		= vk_to_keycode((int)_wparam);
			e.state		= _msg == WM_KEYDOWN
						? mouse_event::status::press
						: mouse_event::status::release;
			if (e.key == key_code::unkown)
			{
				break;
			}
			fire_input(&e);
			break;
		}

		case WM_MOUSEWHEEL :
		{
			POINT	pt	= {GET_X_LPARAM(_lparam), GET_Y_LPARAM(_lparam)};
			ScreenToClient(_hwnd, &pt);
			short				z_delta	= GET_WHEEL_DELTA_WPARAM(_wparam);
			float				z_roll	= (float)z_delta / (float)WHEEL_DELTA;
			mouse_wheel_event	e;
			e.point.x			= pt.x;
			e.point.y			= pt.y;
			e.axis				= mouse_wheel_event::axis_y;
			e.delta				= z_roll;
			fire_input(&e);
			break;
		}

		case WM_LBUTTONDOWN :
		case WM_LBUTTONUP :
		case WM_MBUTTONDOWN :
		case WM_MBUTTONUP :
		case WM_RBUTTONDOWN :
		case WM_RBUTTONUP :
		case WM_MOUSEMOVE :
		{
			mouse_event	e;
			switch (_msg)
			{
				case WM_LBUTTONDOWN :
					e.buttons	= mouse_event::left;
					e.state		= mouse_event::status::press;
					add_capture();
					break;
					
				case WM_LBUTTONUP :
					e.buttons	= mouse_event::left;
					e.state		= mouse_event::status::release;
					dec_capture();
					break;

				case WM_RBUTTONDOWN :
					e.buttons	= mouse_event::right;
					e.state		= mouse_event::status::press;
					add_capture();
					break;
					
				case WM_RBUTTONUP :
					e.buttons	= mouse_event::right;
					e.state		= mouse_event::status::release;
					dec_capture();
					break;

				case WM_MBUTTONDOWN :
					e.buttons	= mouse_event::center;
					e.state		= mouse_event::status::press;
					add_capture();
					break;

				case WM_MBUTTONUP :
					e.buttons	= mouse_event::center;
					e.state		= mouse_event::status::release;
					dec_capture();
					break;

				case WM_MOUSEMOVE :
					e.buttons	= (_wparam & MK_LBUTTON ? mouse_event::left   : 0)
								| (_wparam & MK_RBUTTON ? mouse_event::right  : 0)
								| (_wparam & MK_RBUTTON ? mouse_event::center : 0)
					;
					e.state		= e.buttons ? mouse_event::status::drag : mouse_event::status::move;
					break;
			}
			e.point		= mouse_event::point_type(GET_X_LPARAM(_lparam), GET_Y_LPARAM(_lparam));
			fire_input(&e);
			break;
		}

		case WM_SETTINGCHANGE :
		{
			bool	is = false;
			if (_lparam && CompareStringOrdinal(reinterpret_cast<LPCWCH>(_lparam), -1, L"ImmersiveColorSet", -1, TRUE) == CSTR_EQUAL)
			{
				RefreshImmersiveColorPolicyState();
				is	= true;
			}
			GetIsImmersiveColorUsingHighContrast(IHCM_REFRESH);
			break;
		}


		// WM_WINDOWPOSCHANGINGをフックし、ウィンドウの位置とサイズが変更されようとしているときに
		// 画像がぼやけないようにクライアント領域を偶数に調整します。
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

		case WM_SIZE :
		{
			fire_sizechanged(size());
			break;
		}
/*
		case WM_PAINT :
		{
			HDC			hdc;
			PAINTSTRUCT	ps;
			hdc		= BeginPaint(_hwnd, &ps);
			if (mRepaintCallback)
			{
				mRepaintCallback();
			}
			else
			{
				rect<int>	r{
					{(int)ps.rcPaint.left,
					(int)ps.rcPaint.top},
					{(int)(ps.rcPaint.right-ps.rcPaint.left),
					(int)(ps.rcPaint.bottom-ps.rcPaint.top)}
				};
				fire_repaint(r);
			}
			EndPaint(_hwnd, &ps);
			break;
		}
*/
	}
	return	DefWindowProc(_hwnd, _msg, _wparam, _lparam);
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
