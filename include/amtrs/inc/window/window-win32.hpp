/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__os__win32__window__hpp
#define	__libamtrs__os__win32__window__hpp
#include "../../logging.hpp"
#include "../../os/win32/def.hpp"
#include "../../os/win32/error_code.hpp"
#include <windowsx.h>
AMTRS_NAMESPACE_BEGIN

template<class BaseT>
class	window_win32
		: public BaseT
{
	using	_base_type	= BaseT;
public:
	using	size_type	= typename _base_type::size_type;

	HWND hwnd() const noexcept { return mWnd; }
	HDC  hdc() const noexcept { return mDC; }

	void initialize(size_type _size, LPCSTR _classname, LPCSTR _caption)
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
			if (ptr)
			{
				return	reinterpret_cast<window_win32*>(ptr)->wndproc(_hwnd, _msg, _wparam, _lparam);
			}
			if (_msg == WM_NCCREATE)
			{
				// ウィンドウが作成されたので自身を登録
				// Sample => https://msdn.microsoft.com/ja-jp/library/windows/desktop/ff381400(v=vs.85).aspx
				CREATESTRUCT*	cs		= reinterpret_cast<CREATESTRUCT*>(_lparam);
				window_win32*	thiz	= reinterpret_cast<window_win32*>(cs->lpCreateParams);
				SetWindowLongPtr(_hwnd, GWLP_USERDATA, (LONG_PTR)thiz);
				thiz->mDC	= GetDC(_hwnd);
			}
			return	DefWindowProc(_hwnd, _msg, _wparam, _lparam);
		}; // wcls.lpfnWndProc


		if (!RegisterClassEx(&wcls))
		{
			throw	std::system_error(os::win32::make_last_error_code(), "RegisterClassEx()");
		}

		DWORD	dwStyle		= WS_OVERLAPPEDWINDOW;
		DWORD	dwExStyle	= WS_EX_APPWINDOW;


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
					nullptr,
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

	virtual void* get_context() const noexcept override
	{
		return	mWnd;
	}
	virtual size_type size() const noexcept override
	{
		RECT	rect;
		GetClientRect(mWnd, &rect);
		return	size_type(rect.right - rect.left, rect.bottom - rect.top);
	}

protected:
	static key_code vk_to_keycode(int _vk)
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


	virtual LRESULT wndproc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
	{
		switch (_msg)
		{
			case WM_NCDESTROY :
			{
				// ウィンドウが破棄されたので自身を削除
				SetWindowLongPtr(_hwnd, GWLP_USERDATA, (LONG_PTR)nullptr);
				PostQuitMessage(0);
				break;
			}

			case WM_KEYDOWN :
			case WM_KEYUP :
			{
				if (_base_type::onInputEvent)
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
					_base_type::onInputEvent(&e);
				}
				break;
			}

			case WM_MOUSEWHEEL :
			{
				if (_base_type::onInputEvent)
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
					_base_type::onInputEvent(&e);
				}
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
				if (_base_type::onInputEvent)
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
					_base_type::onInputEvent(&e);
				}
				break;
			}

			case WM_SIZE :
			{
				if (_base_type::m_eventCallback)
				{
					typename _base_type::size_event	e(size());
					_base_type::m_eventCallback(_base_type::event_type::size_changed, &e);
				}
				break;
			}

		}
		return	DefWindowProc(_hwnd, _msg, _wparam, _lparam);
	}

	void add_capture()
	{
		if (mCaptureCount == 0)
		{
			SetCapture(mWnd);
		}
		++mCaptureCount;
	}

	void dec_capture()
	{
		if (mCaptureCount > 0)
		{
			if (--mCaptureCount == 0)
			{
				ReleaseCapture();
			}
		}
	}

	HWND		mWnd			= nullptr;
	HDC			mDC				= nullptr;
	std::size_t	mCaptureCount	= 0;
};


AMTRS_NAMESPACE_END
#endif
