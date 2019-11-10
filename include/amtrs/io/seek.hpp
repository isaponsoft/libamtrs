/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__io__seek__hpp
#define	__libamtrs__io__seek__hpp
#include <iosfwd>
#include "def.hpp"
AMTRS_IO_NAMESPACE_BEGIN



template<class CharT, class Traits>
bool seek(std::basic_istream<CharT, Traits>& _in, std::ios::off_type _off, std::ios::seekdir _dir)
{
	_in.seekg(_off, _dir);
	return	true;
}




AMTRS_IO_NAMESPACE_END
#endif
