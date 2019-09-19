/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__window__dialog_datetime_win32__hpp
#define	__libamtrs__window__dialog_datetime_win32__hpp
#include <functional>
#include "def.hpp"
#include "window.hpp"
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")
AMTRS_NAMESPACE_BEGIN


class	win32_yes_no_dialog_base
		: public window_win32<window>
{
public:
	enum button_ids
	{
		ok,
		cancel,
	};

protected:
	virtual LRESULT wndproc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) override
	{
		switch (_msg)
		{
			case WM_CREATE :
			{
				mCancelBtn = CreateWindow(
				    "BUTTON", "CANCEL", 
				    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				    20, 20, 100, 30, _hwnd, (HMENU)button_ids::cancel, GetModuleHandle(NULL) ,NULL);
				mOkBtn = CreateWindow(
				    "BUTTON", "OK",
				    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				    130, 20, 100, 30, _hwnd, (HMENU)button_ids::ok, GetModuleHandle(NULL), NULL);
				break;
			}

			case WM_COMMAND :
			{
				switch(LOWORD(_wparam))
				{
				    case button_ids::cancel:
						DestroyWindow(_hwnd);
				        break;

				    case button_ids::ok:
						DestroyWindow(_hwnd);
				        break;
				}
				break;
			}

			case WM_SIZE :
			{
				RECT		rect;
				GetClientRect(mWnd, &rect);
				size2<int>	sz(rect.right - rect.left, rect.bottom - rect.top);


				break;
			}
		}
		return	DefWindowProc(_hwnd, _msg, _wparam, _lparam);
	}

private:
	HWND	mCancelBtn		= nullptr;
	HWND	mOkBtn			= nullptr;
};



AMTRS_NAMESPACE_END
#endif
