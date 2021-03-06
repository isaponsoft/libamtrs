/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem.hpp>
#include <amtrs/system.hpp>

AMTRS_NAMESPACE_BEGIN
using namespace filesystem;

static bool is_sep(char c) { return c == '\\' || c == '/'; }

static bool make(amtrs_bufferif_one_init _alloc, std::string _dir)
{
	std::string	dir		= normalize_path(_dir);
	auto*		data	= reinterpret_cast<char*>(_alloc.allocate(_alloc.object, dir.size()));
	if (!data)
	{
		return	false;
	}
	std::copy_n(dir.data(), dir.size(), data);
	return	true;
}



//! プラットフォームの特殊なファイルパスを取得します。
bool special_path(amtrs_bufferif_one_init _destinate, specialpathtype _type, std::string_view _appname)
{
	// https://specifications.freedesktop.org/basedir-spec/latest/ar01s03.html
	if (_type == specialpathtype::home)
	{
		return	make(_destinate, getenv<std::string>("HOME"));
	}
	else
	{
		std::string	home	= getenv<std::string>("XDG_DATA_HOME");
		if (home.empty())
		{
			home	= getenv<std::string>("HOME");
		}
		if (_type == specialpathtype::cache)
		{
			std::string	cacheDir	= getenv<std::string>("XDG_CACHE_HOME");
			if (cacheDir.empty())
			{
				cacheDir	= home + std::string("/.cache/") + std::string(_appname);
			}
			return	make(_destinate, cacheDir);
		}
		if (_type == specialpathtype::config)
		{
			std::string	cacheDir	= getenv<std::string>("XDG_CONFIG_HOME");
			if (cacheDir.empty())
			{
				cacheDir	= home + std::string("/.cache/") + std::string(_appname);
			}
			return	make(_destinate, cacheDir);
		}
		if (_type == specialpathtype::app_local || _type == specialpathtype::app_backup)
		{
			std::string	localDir	= getenv<std::string>("XDG_DATA_HOME");
			if (localDir.empty())
			{
				localDir	= home + std::string("/.local/share/") + std::string(_appname);
			}
			return	make(_destinate, localDir);
		}
	}
	return	false;
}

AMTRS_NAMESPACE_END
