/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__logging__logging_freebsd__hpp
#define	__libamtrs__logging__logging_freebsd__hpp
#include <iostream>
AMTRS_LOGGING_NAMESPACE_BEGIN

inline auto config::max_size() noexcept -> size_type
{
	return	nosize;
}


inline void config::output(const char* _msg, size_type _size)
{
	::fwrite(_msg, 1, (unsigned int)_size, stdout);
	::fputc('\n', stdout);
//	std::cout << std::string_view(_msg, _size) << std::endl;
//	std::cout.flush();
}

AMTRS_LOGGING_NAMESPACE_END
#endif
