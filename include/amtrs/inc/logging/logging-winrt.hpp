/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
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
