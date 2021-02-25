/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__platforms__x11__input_keyboard__hpp
#define	__libamtrs__platforms__x11__input_keyboard__hpp
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <amtrs/input/key_code.hpp>
AMTRS_NAMESPACE_BEGIN

key_code x11key_to_keycode(Display* _display, int _xk);

AMTRS_NAMESPACE_END
#endif
