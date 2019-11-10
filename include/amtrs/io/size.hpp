/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__io__size__hpp
#define	__libamtrs__io__size__hpp
#include <iosfwd>
#include "def.hpp"
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
