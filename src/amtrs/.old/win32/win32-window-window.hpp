/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__os__win32__window__hpp
#define	__libamtrs__os__win32__window__hpp
#include "win32-api-windows.hpp"
#include "../../window.hpp"
AMTRS_NAMESPACE_BEGIN


class	window_win32
		: public window
{
public:
	using	super_type	= window;
	using	size_type	= typename super_type::size_type;

	static inline size_t	sWindowCount;

	window_win32(std::function<typename super_type::handler_func> _handler)
		: super_type(std::move(_handler))
	{}

	HWND hwnd() const noexcept { return mWnd; }
	HDC  hdc() const noexcept { return mDC; }

	void initialize(HWND _parent, size_type _size, LPCSTR _classname, LPCSTR _caption);

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


	bool invoke(std::function<typename super_type::handler_func> const& _handler, HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);

protected:
	static key_code vk_to_keycode(int _vk);

	LRESULT wndproc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);


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

	bool repaint(std::function<void()> _repevent) override
	{
		mRepaintCallback	= std::move(_repevent);
		::InvalidateRect(mWnd, NULL, NULL);
		return	true;
	}

	HWND					mWnd			= nullptr;
	HDC						mDC				= nullptr;
	std::size_t				mCaptureCount	= 0;
	std::function<void()>	mRepaintCallback;

	friend	window;
};




AMTRS_NAMESPACE_END
#endif
