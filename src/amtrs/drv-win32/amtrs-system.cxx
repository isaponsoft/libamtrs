/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <shlobj.h>
#include <amtrs/system/env.hpp>
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "User32.lib")
AMTRS_SYSTEM_NAMESPACE_BEGIN

static bool is_sep(char c) { return c == '\\' || c == '/'; }


bool get_execute_path(amtrs_bufferif_one_init _destinate, char const* _argv0)
{
	simplearray<char>	buf;
	buf.resize(MAX_PATH);
	for (;;)
	{
		DWORD	sz	= GetModuleFileNameA(nullptr, buf.data(), (DWORD)buf.size());
		if (sz > 0)
		{
			DWORD	err	= GetLastError();
			if (err == ERROR_INSUFFICIENT_BUFFER)
			{
				buf.resize(buf.size() * 2);
				continue;
			}
			buf.resize(sz);
			break;
		}
		return	false;
	}
	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, buf.size()));
	if (!data)
	{
		return	false;
	}
	std::copy_n(buf.data(), buf.size(), data);
	return	true;
}


bool getenv(amtrs_bufferif_one_init _destinate, text _name)
{
	sstring		name(_name);
	size_t		buffSize;
	getenv_s(&buffSize, nullptr, 0, name.c_str());
	if (buffSize == 0)
	{
		return	false;
	}

	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, buffSize-1));	// eitout \0.
	if (!data)
	{
		return	false;
	}
	getenv_s(&buffSize, data, buffSize, name.c_str());	// with \0.
	return	true;
}


void setenv(text _key, text _value)
{
	sstring		env(_key);
	env.append("=");
	env.append(_value);
	_putenv(env.c_str());
}


text env_separator()
{
	return	";";
}


//! プラットフォームの特殊なファイルパスを取得します。
bool special_path(amtrs_bufferif_one_init _destinate, specialpathtype _type, text _appname)
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

	sstring	dir;
	dir.append(rootDir);
	dir.append("\\");
	dir.append(_appname);
//	dir	= filesystem::normalize_path(dir);

	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, dir.size()));
	if (!data)
	{
		return	false;
	}
	std::copy_n(dir.data(), dir.size(), data);
	return	true;
}


bool hostname(amtrs_bufferif_one_init _destinate)
{
	char	buff[100];
	DWORD	size	= sizeof(buff);
	GetComputerNameA(buff, &size);	// size not include NUL
	for (DWORD i = 0; i < size; ++i)
	{
		if (buff[i] >= 'A' && buff[i] <= 'Z')
		{
			buff[i]	= buff[i] - 'A' + 'a';
		}
	}
	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, size));
	if (!data)
	{
		return	false;
	}
	std::copy_n(buff, size, data);
	return	true;
}


bool username(amtrs_bufferif_one_init _destinate)
{
	char	buff[100];
	DWORD	size	= sizeof(buff);
	GetUserNameA(buff, &size);			// size include NUL
	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, size-1));
	if (!data)
	{
		return	false;
	}
	std::copy_n(buff, size-1, data);
	return	true;
}

AMTRS_SYSTEM_NAMESPACE_END
