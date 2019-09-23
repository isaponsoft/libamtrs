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
#ifndef	__libamtrs__window__display_win32__hpp
#define	__libamtrs__window__display_win32__hpp
#include "../../os/win32.hpp"
#include "display.hpp"
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
