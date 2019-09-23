/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__io__types__hpp
#define	__libamtrs__io__types__hpp
#include "def.hpp"
AMTRS_IO_NAMESPACE_BEGIN


enum class	seek_origin
{
	so_begin,
	so_cur,
	so_end,
};


AMTRS_IO_NAMESPACE_END
#endif
