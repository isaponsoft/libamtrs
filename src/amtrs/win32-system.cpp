/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/system.hpp>
#include <shlobj.h>


AMTRS_NAMESPACE_BEGIN

static bool is_sep(char c) { return c == '\\' || c == '/'; }


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


	size_t	sizeRoot		= strlen(rootDir);
	size_t	addRootSep		= is_sep(rootDir[sizeRoot-1]) ? 0 : 1;
	size_t	sizeAppname		= _appname.size();
	size_t	addAppSep		= is_sep(_appname[_appname.size()-1]) ? 0 : 1;


	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, sizeRoot + addRootSep + sizeAppname + addAppSep));
	if (!data)
	{
		return	false;
	}

	std::copy_n(rootDir, sizeRoot, data);
	data += sizeRoot;
	if (addRootSep)
	{
		*data++ = '\\';
	}

	std::copy_n(_appname.data(), sizeAppname, data);
	data += sizeAppname;
	if (addAppSep)
	{
		*data++ = '\\';
	}

	return	true;
}

AMTRS_NAMESPACE_END
