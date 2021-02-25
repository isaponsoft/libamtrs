/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__platforms__posix__io_functions__hpp
#define	__libamtrs__platforms__posix__io_functions__hpp
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
AMTRS_IO_NAMESPACE_BEGIN

static constexpr int	_APPEND			= O_APPEND;
#ifdef	O_BINARY
static constexpr int	_BINARY			= O_BINARY;
#else
static constexpr int	_BINARY			= 0;
#endif
static constexpr int	_CREAT			= O_CREAT;
#ifdef	O_SHORT_LIVED
static constexpr int	_SHORT_LIVED	= O_SHORT_LIVED;
#endif
#ifdef	O_TEMPORARY
static constexpr int	_TEMPORARY		= O_TEMPORARY;
#endif
#ifdef	O_NOINHERIT
static constexpr int	_NOINHERIT		= O_NOINHERIT;
#endif
static constexpr int	_RDONLY			= O_RDONLY;
static constexpr int	_RDWR			= O_RDWR;
#ifdef	O_TEXT
static constexpr int	_TEXT			= O_TEXT;
#endif
static constexpr int	_TRUNC			= O_TRUNC;
static constexpr int	_WRONLY			= O_WRONLY;

int open(char const* _filename, int _oflag)
{
	return	::open(_filename, _oflag); 
}

int open(char const* _filename, int _oflag, mode_t _pmode)
{
	return	::open(_filename, _oflag, _pmode); 
}

int read(int const _fd, void * const _buffer, unsigned const _buffer_size)
{
	return	::read(_fd, _buffer, _buffer_size); 
}

int write(int const _fd, void const * const _buffer, unsigned int _count)
{
	return	::write(_fd, _buffer, _count); 
}

int close(int const _fd)
{
	return	::close(_fd); 
}


AMTRS_IO_NAMESPACE_END
#endif
