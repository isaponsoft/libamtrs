/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__read_checksum__hpp
#define	__libamtrs__io__read_checksum__hpp
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
