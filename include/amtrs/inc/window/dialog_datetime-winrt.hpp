/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__window__dialog_datetime_winrt__hpp
#define	__libamtrs__window__dialog_datetime_winrt__hpp
#include <functional>
#include "def.hpp"
#include "window.hpp"
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

};



AMTRS_NAMESPACE_END
#endif
