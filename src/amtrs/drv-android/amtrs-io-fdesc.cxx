/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <amtrs/io/fdesc.hpp>
AMTRS_IO_NAMESPACE_BEGIN


fdesc fdesc::stdo()
{
	return	fdesc(fileno(stdout));
}

fdesc fdesc::stde()
{
	return	fdesc(fileno(stderr));
}

fdesc fdesc::stdi()
{
	return	fdesc(fileno(stdin));
}

errorval fdesc::open(char const* _filename, open_mode _mode)
{
	int	f	= ::open(_filename, 0);
	if (f < 0)
	{
		return	errorval(errno);
	}
	mDesc	= f;
	return	{};
}


errorval fdesc::create(char const* _filename, open_mode _mode)
{
	int	f	= ::open(_filename, O_CREAT|O_TRUNC, 0664);
	if (f < 0)
	{
		return	errorval(errno);
	}
	mDesc	= f;
	return	{};
}

stat fdesc::status() const noexcept
{
	if (!empty())
	{
		struct ::stat	s;
		if (!::fstat(get(), &s))
		{
			return	{ (size_t)s.st_size, (uint64_t)s.st_atime, (uint64_t)s.st_mtime, (uint64_t)s.st_ctime };
		}
	}
	return	{ (size_t)-1, 0, 0, 0 };
}

int fdesc::read(void* _buff, int _size)
{
	if (empty())
	{
		return	0;
	}
	return	::read(mDesc, _buff, _size);
}

int fdesc::write(void const* _buff, int _size)
{
	if (empty())
	{
		return	0;
	}
	return	::write(mDesc, _buff, _size);
}

void fdesc::close()
{
	if (!empty())
	{
		::close(mDesc);
		mDesc	= -1;
	}
}


AMTRS_IO_NAMESPACE_END
