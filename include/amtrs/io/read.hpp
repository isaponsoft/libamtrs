/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__io__read__hpp
#define	__libamtrs__io__read__hpp
#include <iosfwd>
#include "../memory/endian.hpp"
#include "def.hpp"
#include "io_traits.hpp"
#include "size.hpp"
AMTRS_IO_NAMESPACE_BEGIN


template<endian E, class T, class In>
bool read(T& _buff, In&& _in)
{
	if (io_traits_t<In>{}.read(&_buff, _in, sizeof(T)) != sizeof(T))
	{
		return	false;
	}
	_buff	= endian_util::decode<E>(_buff);
	return	true;
}


template<endian E, class T, class In, class SIZE>
SIZE read(T& _buff, In&& _in, SIZE _size)
{
	return	io_traits_t<In>{}.read(&(_buff[0]), _in, _size);
}


template<class T, class In, class SIZE>
SIZE read(T* _buff, In&& _in, SIZE _size)
{
	return	io_traits_t<In>{}.read(_buff, _in, _size);
}


template<size_t SIZE, class In>
size_t read(char (&_buff)[SIZE], In&& _in)
{
	return	io_traits_t<In>{}.read(&(_buff[0]), _in, SIZE);
}









template<class T, class In>
T readall(In&& _io)
{
	auto	s	= size(_io);
	T		retval(s);
	s	= read(retval.data(), _io, s);
	return	retval;
}


AMTRS_IO_NAMESPACE_END
#endif
