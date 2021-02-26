/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__logging__logging_winrt__hpp
#define	__libamtrs__logging__logging_winrt__hpp
#include <iostream>
#include <windows.h>
AMTRS_LOGGING_NAMESPACE_BEGIN

inline auto config::max_size() noexcept -> size_type
{
	return	nosize;
}


inline void config::output(const char* _msg, size_type _size)
{
	(void)_size;
#ifdef	_CONSOLE
	std::cout << std::string_view(_msg, _size) << std::endl;
	std::cout.flush();
#else
	OutputDebugStringA(_msg);	//	Visual Studio.
	OutputDebugStringA("\n");	//	Visual Studio.
#endif
}



AMTRS_LOGGING_NAMESPACE_END
#endif
