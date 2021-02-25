/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__platforms__x11__window_window__hpp
#define	__libamtrs__platforms__x11__window_window__hpp
#include <amtrs/window/display.hpp>
#include "input-keyboard.hpp"
#include "window-window-xcb.hpp"
AMTRS_NAMESPACE_BEGIN


inline ref<window> window::create(size_type _size)
{
	ref<window_xcb<window>>	w	= new window_xcb<window>();
	w->initialize(_size);
	return	w;
}


AMTRS_NAMESPACE_END
#endif
