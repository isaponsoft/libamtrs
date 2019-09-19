/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__web__web_win32__hpp
#define	__libamtrs__web__web_win32__hpp
#include "../../os/win32/windows.hpp"
#include "../../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN

struct	web
{
	static void jump_to(const std::string& _url)
	{
		::ShellExecute(NULL, "open", _url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
};



AMTRS_NAMESPACE_END
#endif
