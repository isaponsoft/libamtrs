/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__size__hpp
#define	__libamtrs__io__size__hpp
AMTRS_IO_NAMESPACE_BEGIN



template<class In>
uintmax_t size(In& _in)
{
	return	io_traits_t<In>{}.size(_in);
}



template<class CharT, class Traits>
uintmax_t size(std::basic_istream<CharT, Traits>& _in)
{
	_in.clear();
	seek(_in, 0, std::ios::end);
	auto	endPos	= _in.tellg();
	_in.clear();
	seek(_in, 0, std::ios::beg);
	auto	begPos	= _in.tellg();
	_in.clear();
	return	static_cast<uintmax_t>(endPos - begPos);
}



AMTRS_IO_NAMESPACE_END
#endif
