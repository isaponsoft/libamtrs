/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__filesystem__functions__win32__hpp
#define	__libamtrs__filesystem__functions__win32__hpp
#include "../string.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


inline std::string current_path()
{
	DWORD		size	= GetCurrentDirectory(0, NULL);
	std::string	buff;
	buff.resize(static_cast<size_t>(size));
	GetCurrentDirectory(static_cast<DWORD>(buff.size()), buff.data());
	buff.resize(static_cast<size_t>(size)-1);
	return	buff;
}


inline bool create_directory(path_type _path, bool _recursive = true)
{
	if (_recursive)
	{
		std::string	dirname;
		for (auto dir : split_chars(std::string_view(_path), "/\\"))
		{
			dirname += std::string(dir);
			if ((::_mkdir(dirname.c_str()) != 0) && (errno != EEXIST))
			{
				return	false;
			}
			dirname	+= "\\";
		}
	}
	else
	{
		std::string	p(_path);
		if (::_mkdir(p.c_str()) != 0)
		{
			return	false;
		}
	}
	return	true;
}

AMTRS_FILESYSTEM_NAMESPACE_END
#endif
