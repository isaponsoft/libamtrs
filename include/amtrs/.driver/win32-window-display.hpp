/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__window__display_win32__hpp
#define	__libamtrs__window__display_win32__hpp
#include "win32-api-windows.hpp"
AMTRS_NAMESPACE_BEGIN

class	win32_display : public display
{
public:
	virtual float pixel_par_inch() override
	{
		return	-1;
	}


	virtual size2<int> size() override
	{
		RECT	ws;
		::GetWindowRect(::GetDesktopWindow(), &ws);
		return	size2<int>(ws.right - ws.left, ws.bottom - ws.top);
	}

	virtual float pt_to_pixel_scale() override
	{
		return	1;
	}

	friend	display;
};


inline ref<display> display::get_display(int _display_number)
{
	if (_display_number != 0)
	{
		throw	std::exception();
	}
	ref<win32_display>	thiz = new win32_display();
	return	thiz;
}

AMTRS_NAMESPACE_END
#endif
