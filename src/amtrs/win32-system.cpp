/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem.hpp>
#include <amtrs/system.hpp>
#include <shlobj.h>


AMTRS_NAMESPACE_BEGIN

static bool is_sep(char c) { return c == '\\' || c == '/'; }


void setenv(std::string_view _key, std::string_view _value)
{
	std::string	env(_key);
	env += "=";
	env += _value;
	_putenv(env.c_str());
/*	std::string	key(_key);
	std::string	val(_value);
	SetEnvironmentVariableA(key.c_str(), val.c_str());
*/
}


//! プラットフォームの特殊なファイルパスを取得します。
bool special_path(amtrs_bufferif_one_init _destinate, specialpathtype _type, std::string_view _appname)
{

	int		csidl	= CSIDL_LOCAL_APPDATA;
	switch (_type)
	{
		case specialpathtype::home:			csidl = CSIDL_PROFILE; break;
		case specialpathtype::cache:		csidl = CSIDL_LOCAL_APPDATA; break;
		case specialpathtype::app_local:	csidl = CSIDL_LOCAL_APPDATA; break;
		case specialpathtype::app_backup:	csidl = CSIDL_APPDATA; break;
		case specialpathtype::config:		csidl = CSIDL_LOCAL_APPDATA; break;
	}

	char	rootDir[MAX_PATH];
	SHGetSpecialFolderPathA(nullptr, rootDir, CSIDL_LOCAL_APPDATA, 0);

	std::string	dir;
	dir	+= rootDir;
	dir += "\\";
	dir += _appname;
	dir	= filesystem::normalize_path(dir);

	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, dir.size()));
	if (!data)
	{
		return	false;
	}
	std::copy_n(dir.data(), dir.size(), data);
	return	true;
}

AMTRS_NAMESPACE_END
