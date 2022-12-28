/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__io_traits_unixsystemlike__hpp
#define	__libamtrs__io__io_traits_unixsystemlike__hpp
#include <iosfwd>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
AMTRS_IO_NAMESPACE_BEGIN


template<>
struct	io_traits<int>
{
	using	value_type	= int;
	using	size_type	= std::size_t;
	using	fpos_type	= int64_t;

	static constexpr value_type	nio	= -1;

	template<class T>
	value_type open(T _device, int _mode)
	{
		int	m	= O_RDWR;
		#ifdef	O_BINARY
		m |= O_BINARY;
		#endif
		return	::open(_device, m);
	}

	size_type read(value_type _io, void* _buffer, size_type _request)
	{
		return	::read(_io, _buffer, _request);
	}

	size_type write(value_type _io, const void* _buffer, size_type _request)
	{
		return	::write(_io, _buffer, _request);
	}

	fpos_type seek(value_type _io, fpos_type _position, std::ios::seekdir _org)
	{
		int	org	= _org == std::ios::beg ? 0
				: _org == std::ios::end   ? 2
				: 1;
		return	::lseek(_io, _position, org);
	}


	size_type size(value_type _io)
	{
		struct stat	st;
		::fstat(_io, &st);
		return	st.st_size;
	}


	void close(value_type _io)
	{
		::close(_io);
	}

	bool exists(const char* _devicename)
	{
		struct stat	st;
		return	::stat(_devicename, &st) == 0;
	}
};


AMTRS_IO_NAMESPACE_END
#endif
