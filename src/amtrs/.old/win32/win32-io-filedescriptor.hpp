/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__platforms__win32__io_filedescriptor__hpp
#define	__libamtrs__platforms__win32__io_filedescriptor__hpp
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
AMTRS_IO_NAMESPACE_BEGIN


inline auto fdesc::_open(char const* _filename, openmode _mode, accessmode _access) -> value_type
{
	int		fd;
	int		shflg	= _SH_DENYNO;
	int		pmode	= _S_IREAD | _S_IWRITE;
	int		amode	= _access == accessmode::read_only  ? _O_RDONLY
					: _access == accessmode::write_only ? _O_WRONLY
					:                                     _O_RDWR;
	switch (_mode)
	{
		case openmode::open :
		{
			if (!::_sopen_s(&fd, _filename, _O_BINARY|amode, shflg, pmode))
			{
				return	fd;
			}
			break;
		}

		case openmode::create :
		{
			if (!::_sopen_s(&fd, _filename, _O_CREAT|_O_EXCL|_O_BINARY|amode, shflg, pmode))
			{
				return	fd;
			}
			break;
		}

		case openmode::open_always :
		{
			if (!::_sopen_s(&fd, _filename, _O_CREAT|_O_BINARY|amode, shflg, pmode))
			{
				return	fd;
			}
			break;
		}

		case openmode::create_always :
		{
			if (!::_sopen_s(&fd, _filename, _O_CREAT|_O_TRUNC|_O_BINARY|amode, shflg, pmode))
			{
				return	fd;
			}
			break;
		}
	}
	return	nval;
}



inline fdesc& fdesc::_read(void* _buff, size_type _requestsize)
{
	if (good())
	{
		int		r	= ::_read(mDesc, _buff, _requestsize); 
		if (r >= 0)
		{
			mGCount	= r;
		}
		else
		{
			setstate(eofbit);
			mGCount	= 0;
		}
	}
	return	*this;
}

inline fdesc& fdesc::_write(void const* _buff, size_type _requestsize)
{
	if (good())
	{
		int		r	= ::_write(mDesc, _buff, _requestsize); 
		if (r >= 0)
		{
			mPCount	= r;
		}
		else
		{
			mPCount	= 0;
		}
	}
	return	*this;
}

inline void fdesc::_seek(off_type _off, seekdir _dir)
{
	if (!good())
	{
		return;
	}
	int o	= _dir == seekdir_cur ? SEEK_CUR
			: _dir == seekdir_end ? SEEK_END
			:                       SEEK_SET;
	::_lseek(mDesc, _off, o);
}


inline fdesc::off_type fdesc::_tell()
{
	if (!good())
	{
		return	0;
	}
	return	::_tell(mDesc);
}


inline void fdesc::_close()
{
	::_close(mDesc); 
}


AMTRS_IO_NAMESPACE_END
#endif
