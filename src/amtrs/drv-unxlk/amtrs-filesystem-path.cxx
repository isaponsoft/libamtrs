/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <amtrs/string/convert.hpp>
#include <amtrs/string/split_iterator.hpp>
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

text directory_separators() noexcept
{
	return	"/";
}


bool current_path(text _path)
{
	sstring	path(_path);
	if (!chdir(path.c_str()))
	{
		return	true;
	}
	return	false;
}


bool current_path(amtrs_bufferif_one_init _destinate)
{
	char	buff[PATH_MAX];
	char*	r	= getcwd(buff, PATH_MAX);
	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, strlen(buff)));
	if (!data)
	{
		return	false;
	}
	std::copy_n(buff, strlen(buff), reinterpret_cast<char*>(data));
	return	true;
}


bool is_directory(text _path)
{
	struct stat	st;
	sstring	path(_path);
	return	!stat(path.c_str(), &st) && S_ISDIR(st.st_mode);
}



bool readlink(amtrs_bufferif_one_init _destinate, text _path) noexcept
{
	char	buf[PATH_MAX+1]	;
	int		len	= ::readlink(sstring(_path).c_str(), buf, sizeof(buf));
	if (len < 0)
	{
		return	false;
	}
	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, len));
	if (!data)
	{
		return	false;
	}
	std::copy_n(buf, len, data);
	return	true;
}


text root_name(text _path) noexcept
{
	if (_path.front() == '/')
	{
		return	_path.substr(0, 1);
	}
	return	_path.substr(0, 0);
}

AMTRS_FILESYSTEM_NAMESPACE_END
