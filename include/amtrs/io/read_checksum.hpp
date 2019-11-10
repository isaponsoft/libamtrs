/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__io__read_checksum__hpp
#define	__libamtrs__io__read_checksum__hpp
#include <cstring>
#include "read.hpp"
AMTRS_IO_NAMESPACE_BEGIN

template<class CharT, class Traits, size_t CS>
bool read_checksum(std::basic_istream<CharT, Traits>& _in, char const (& _csum)[CS])
{
	char	buff[CS];
	auto	rs	= read(buff, _in, CS);
	return	rs == CS
			? (bool)(!std::memcmp(buff, _csum, CS))
			: false;
}


AMTRS_IO_NAMESPACE_END
#endif
