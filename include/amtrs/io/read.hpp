/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__io__read__hpp
#define	__libamtrs__io__read__hpp
#include "def.hpp"
#include "io_traits.hpp"
AMTRS_IO_NAMESPACE_BEGIN



template<class T, class F, class N>
T readall(N&& _devicename)
{
	auto	io	= F(std::forward<N>(_devicename));
	if (io.empty())
	{
		return	{};
	}
	auto	s	= io.size();
	T	retval;
	retval.resize(s);
	s	= io.read(retval.data(), s);
	retval.resize(s);
	return	retval;
}


AMTRS_IO_NAMESPACE_END
#endif
