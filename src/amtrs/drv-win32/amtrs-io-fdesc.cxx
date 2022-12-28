/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <amtrs/io/fdesc.hpp>
AMTRS_IO_NAMESPACE_BEGIN


fdesc fdesc::stdo()
{
	return	fdesc(_fileno(stdout));
}

fdesc fdesc::stde()
{
	return	fdesc(_fileno(stderr));
}

fdesc fdesc::stdi()
{
	return	fdesc(_fileno(stdin));
}

errorval fdesc::append(char const* _filename, unsigned int _shared_mode)
{
	int	f;
	int	oflag	= _O_CREAT|_O_APPEND|_O_BINARY|_O_RDWR;
	int	shflag	= 0;
	int	pmode	= _S_IREAD|_S_IWRITE;
	if (!(_shared_mode & sm_read ))	shflag |= _SH_DENYRD;
	if (!(_shared_mode & sm_write))	shflag |= _SH_DENYWR;
	if (!shflag)					shflag =  _SH_DENYNO;
	if (::_sopen_s(&f, _filename, oflag, shflag, pmode))
	{
		return	errorval(errno);
	}
	mDesc	= f;
	return	errorval();
}


errorval fdesc::open(char const* _filename, open_mode _mode, unsigned int _shared_mode)
{
	int	f;
	int	oflag	= _O_BINARY|_O_RDWR;
	int	shflag	= 0;
	int	pmode	= _S_IREAD|_S_IWRITE;
	if (!(_shared_mode & sm_read ))	shflag |= _SH_DENYRD;
	if (!(_shared_mode & sm_write))	shflag |= _SH_DENYWR;
	if (!shflag)					shflag =  _SH_DENYNO;
	if (::_sopen_s(&f, _filename, oflag, shflag, pmode))
	{
		return	errorval(errno);
	}
	mDesc	= f;
	return	errorval();
}


errorval fdesc::create(char const* _filename, open_mode _mode, unsigned int _shared_mode)
{
	int	f;
	int	oflag	= _O_CREAT|_O_TRUNC|_O_BINARY|_O_RDWR;
	int	shflag	= 0;
	int	pmode	= _S_IREAD|_S_IWRITE;
	if (!(_shared_mode & sm_read ))	shflag |= _SH_DENYRD;
	if (!(_shared_mode & sm_write))	shflag |= _SH_DENYWR;
	if (!shflag)					shflag =  _SH_DENYNO;
	if (::_sopen_s(&f, _filename, oflag, shflag, pmode))
	{
		return	errorval(errno);
	}
	mDesc	= f;
	return	errorval();
}


stat fdesc::status() const noexcept
{
	if (!empty())
	{
		struct _stat	s;
		if (!::_fstat(get(), &s))
		{
			return	{
				static_cast<size_t>(s.st_size),
				static_cast<uint64_t>(s.st_atime),
				static_cast<uint64_t>(s.st_mtime),
				static_cast<uint64_t>(s.st_ctime)
			};
		}
	}
	return	{ static_cast<size_t>(-1), 0, 0, 0 };
}

int fdesc::read(void* _buff, int _size)
{
	if (empty())
	{
		return	0;
	}
	return	::_read(mDesc, _buff, _size);
}

int fdesc::write(void const* _buff, int _size)
{
	if (empty())
	{
		return	0;
	}
	return	::_write(mDesc, _buff, _size);
}

void fdesc::close()
{
	if (!empty())
	{
		::_close(mDesc);
		mDesc	= -1;
	}
}


AMTRS_IO_NAMESPACE_END
