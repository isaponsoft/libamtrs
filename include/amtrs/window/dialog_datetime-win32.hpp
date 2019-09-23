/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
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
