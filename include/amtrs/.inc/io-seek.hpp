/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__seek__hpp
#define	__libamtrs__io__seek__hpp
AMTRS_IO_NAMESPACE_BEGIN



template<class CharT, class Traits>
bool seek(std::basic_istream<CharT, Traits>& _in, std::ios::off_type _off, std::ios::seekdir _dir)
{
	_in.seekg(_off, _dir);
	return	true;
}




AMTRS_IO_NAMESPACE_END
#endif
