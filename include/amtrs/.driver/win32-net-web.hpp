/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__web__web_win32__hpp
#define	__libamtrs__web__web_win32__hpp
#include ".api-windows.hpp"
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
