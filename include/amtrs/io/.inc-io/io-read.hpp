/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__read__hpp
#define	__libamtrs__io__read__hpp
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
	return	(SIZE)io_traits_t<In>{}.read(_buff, _in, _size);
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
