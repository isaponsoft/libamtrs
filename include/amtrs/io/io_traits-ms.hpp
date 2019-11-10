/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__io__io_traits_microsoft__hpp
#define	__libamtrs__io__io_traits_microsoft__hpp
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include "def.hpp"
#include "types.hpp"
AMTRS_IO_NAMESPACE_BEGIN


template<>
struct	io_traits<int>
{
	using	value_type	= int;
	using	size_type	= std::size_t;
	using	fpos_type	= int64_t;

	static constexpr value_type	nio	= -1;

	template<class T>
	value_type open(T _device, int /*_mode*/)
	{
		int	m	= _O_RDWR;
		#ifdef	_O_BINARY
		m |= _O_BINARY;
		#endif
		int		p	= nio;
		auto	r	= ::_sopen_s(&p, _device, m, _SH_DENYRW, _S_IREAD|_S_IWRITE);
		return	r ? nio : p;
	}

	size_type read(value_type _io, void* _buffer, size_type _request)
	{
		return	::_read(_io, _buffer, (unsigned int)_request);
	}

	size_type write(value_type _io, const void* _buffer, size_type _request)
	{
		return	::_write(_io, _buffer, (unsigned int)_request);
	}

	fpos_type seek(value_type _io, fpos_type _position, std::ios::seekdir _org)
	{
		int	org	= _org == std::ios::beg ? 0
				: _org == std::ios::end   ? 2
				: 1;
		return	::_lseek(_io, (long)_position, org);
	}


	size_type size(value_type _io)
	{
		struct stat	st;
		::fstat(_io, &st);
		return	st.st_size;
	}


	void close(value_type _io)
	{
		::_close(_io);
	}

	bool exists(const char* _devicename)
	{
		struct stat	st;
		return	::stat(_devicename, &st) == 0;
	}
};


AMTRS_IO_NAMESPACE_END
#endif
