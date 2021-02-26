/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__platforms__posix__chrono_time__hpp
#define	__libamtrs__platforms__posix__chrono_time__hpp
#include <chrono>
AMTRS_CHRONO_NAMESPACE_BEGIN



inline struct tm gmtime(time_t _t)
{
	return	*::gmtime(&_t);
}

inline struct tm localtime(time_t _t)
{
	static thread_local struct tm	r;
	static thread_local time_t		t	= 0;
	if (t != _t)
	{
		t	= _t;
		r	= *::localtime(&t);
	}
	return	r;
}



AMTRS_CHRONO_NAMESPACE_END
#endif
