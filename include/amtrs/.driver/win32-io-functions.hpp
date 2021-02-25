/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__platforms__win32__io_functions__hpp
#define	__libamtrs__platforms__win32__io_functions__hpp
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
AMTRS_IO_NAMESPACE_BEGIN

static constexpr int	_APPEND			= _O_APPEND;
static constexpr int	_BINARY			= _O_BINARY;
static constexpr int	_CREAT			= _O_CREAT;
static constexpr int	_SHORT_LIVED	= _O_SHORT_LIVED;
static constexpr int	_TEMPORARY		= _O_TEMPORARY;
static constexpr int	_NOINHERIT		= _O_NOINHERIT;
static constexpr int	_RDONLY			= _O_RDONLY;
static constexpr int	_RDWR			= _O_RDWR;
static constexpr int	_TEXT			= _O_TEXT;
static constexpr int	_TRUNC			= _O_TRUNC;
static constexpr int	_WRONLY			= _O_WRONLY;

int open(char const* _filename, int _oflag, int _pmode = 0)
{
	int		fd;
	auto	e	= ::_sopen_s(&fd, _filename, _oflag, 0, _pmode); 
	return	e ? -1 : fd;
}

int read(int const _fd, void * const _buffer, unsigned const _buffer_size)
{
	return	::_read(_fd, _buffer, _buffer_size); 
}

int write(int const _fd, void * const _buffer, unsigned int _count)
{
	return	::_write(_fd, _buffer, _count); 
}

int close(int const _fd)
{
	return	::_close(_fd); 
}


AMTRS_IO_NAMESPACE_END
#endif
