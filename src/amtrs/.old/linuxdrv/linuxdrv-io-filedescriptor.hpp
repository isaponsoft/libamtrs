/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__platforms__linuxdrv__io_filedescriptor__hpp
#define	__libamtrs__platforms__linuxdrv__io_filedescriptor__hpp
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
AMTRS_IO_NAMESPACE_BEGIN


inline auto fdesc::_open(char const* _filename, openmode _mode, accessmode _access) -> value_type
{
	int		fd;
	int		amode	= _access == accessmode::read_only  ? O_RDONLY
					: _access == accessmode::write_only ? O_WRONLY
					:                                     O_RDWR;
	switch (_mode)
	{
		case openmode::open :
		{
			if ((fd = ::open(_filename, amode)) < 0)
			{
				return	fd;
			}
			break;
		}

		case openmode::create :
		{
			if ((fd = ::open(_filename, O_CREAT|O_EXCL|amode)) < 0)
			{
				return	fd;
			}
			break;
		}

		case openmode::open_always :
		{
			if ((fd = ::open(_filename, O_CREAT|amode)) < 0)
			{
				return	fd;
			}
			break;
		}

		case openmode::create_always :
		{
			if ((fd = ::open(_filename, O_CREAT|O_TRUNC|amode)) < 0)
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
		int		r	= ::read(mDesc, _buff, _requestsize); 
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
		int		r	= ::write(mDesc, _buff, _requestsize); 
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
	::lseek(mDesc, _off, o);
}


inline fdesc::off_type fdesc::_tell()
{
	if (!good())
	{
		return	0;
	}
	return	::lseek(mDesc, 0, SEEK_CUR);
}


inline void fdesc::_close()
{
	::close(mDesc); 
}


AMTRS_IO_NAMESPACE_END
#endif
