/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__filesystem__functions__posix__hpp
#define	__libamtrs__filesystem__functions__posix__hpp
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../string.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


inline std::string current_path()
{
	std::string	buff;
	buff.resize(PATH_MAX);
	getcwd(buff.data(), buff.size());
	buff.resize(strlen(buff.c_str()));
	return	buff;
}



inline bool create_directory(path_type _path, bool _recursive = true)
{
	if (_recursive)
	{
		std::string	dirname;
		for (auto dir : split_chars(std::string_view(_path), "/"))
		{
			dirname += std::string(dir);
			if ((::mkdir(dirname.c_str(), 0700) != 0) && (errno != EEXIST))
			{
				return	false;
			}
			dirname	+= "/";
		}
	}
	else
	{
		std::string	p(_path);
		if (::mkdir(p.c_str(), 0700))
		{
			return	true;
		}
	}
	return	false;
}


AMTRS_FILESYSTEM_NAMESPACE_END
#endif
