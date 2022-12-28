/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <windows.h>
#include <amtrs/gui/window.hpp>
AMTRS_OS_NAMESPACE_BEGIN
class	window : public AMTRS_NAMESPACE::window
{
public:
	sstring				mClassName;
	listener*			mListener	= nullptr;
	HDC					mDC			= nullptr;
	HWND				mWnd		= nullptr;
	window_size_type	mSize		= { 0, 0 };
	repaint_callback	mRepaint;

	LRESULT wndproc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);

	void create(createparams& _cp, ref<window> _parent);
	auto repaint(repaint_callback _callback) -> bool;

	sstring gen_classname();
};
AMTRS_OS_NAMESPACE_END
