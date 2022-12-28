/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <algorithm>
#include <errno.h>
#include <amtrs/filesystem/util.hpp>
#include <amtrs/io/fdesc.hpp>
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

errorval file_get_contents(amtrs_bufferif_one_init _destinate, text _path)
{
	io::fdesc	f;
	sstring		path(_path);
	errorval	e	= f.open(path.c_str(), io::fdesc::om_default);
	if (e.bad())
	{
		return	e;
	}
	io::stat	s	= f.status();
	if (s.bad())
	{
		return	errorval(errno);
	}
	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, s.st_size));
	if (!data)
	{
		return	errorval(ENOMEM);
	}
	int		rs		= f.read(data, (int)s.st_size);
	f.close();
	if (rs != s.st_size)
	{
		return	errorval(errno);
	}
	return	errorval();
}


errorval file_put_contents(text _path, void const* _data, size_t _size)
{
	io::fdesc	f;
	sstring		path(_path);
	errorval	e	= f.create(path.c_str(), io::fdesc::om_default);
	if (e.bad())
	{
		return	e;
	}

	int		rs		= f.write(_data, (int)_size);
	f.close();
	if (rs != _size)
	{
		return	errorval(errno);
	}
	return	errorval();
}

AMTRS_FILESYSTEM_NAMESPACE_END
