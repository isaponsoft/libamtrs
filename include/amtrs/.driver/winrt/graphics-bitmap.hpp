/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__bitmaps__bitmap_winwt__hpp
#define	__libamtrs__bitmaps__bitmap_winwt__hpp
#include <atomic>
#include <iostream>
#include "../../os/win32/windows.hpp"
#include "../iostream/rstream.hpp"
#include "../../os/win32/com_istream.hpp"
#pragma comment(lib, "gdiplus.lib")
AMTRS_NAMESPACE_BEGIN

template<class PixelT, class BufferT>
template<class In>
auto basic_bitmap<PixelT, BufferT>::load(In&& _in) -> basic_bitmap
{
	basic_bitmap	retval;

	return	retval;
}

AMTRS_NAMESPACE_END
#endif
