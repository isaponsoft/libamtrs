/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <shlwapi.h>
#include <amtrs/string/convert.hpp>
#include <amtrs/string/split_iterator.hpp>
#pragma comment(lib, "Shlwapi.lib")
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

text directory_separators() noexcept
{
	return	"/\\";
}


bool current_path(text _path)
{
	auto	wpath	= c8_to_wc(_path);
	return	SetCurrentDirectoryW(wpath.c_str())
			? true
			: false;
}

bool current_path(amtrs_bufferif_one_init _destinate)
{
	DWORD	size	= GetCurrentDirectory(0, NULL);
	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, size-1));
	if (!data)
	{
		return	false;
	}
	GetCurrentDirectoryA(size, data);
	return	true;
}

bool is_directory(text _path)
{
	wsstring	path	= c8_to_wc(sstring(_path).replace("/", "\\"));
	return	PathIsDirectoryW(path.c_str())
			? true
			: false;
}


bool readlink(amtrs_bufferif_one_init _destinate, text _path) noexcept
{
	wsstring	path	= c8_to_wc(_path);
	wchar_t		buf[MAX_PATH+1]	;

	HANDLE	f	= CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (f == INVALID_HANDLE_VALUE)
	{
		return	false;
	}
	DWORD	len	= GetFinalPathNameByHandleW(f, buf, MAX_PATH, 0);
	CloseHandle(f);
	if (!len)
	{
		return	false;
	}
	buf[len]	= 0;
	sstring	last	= wc_to_c8(buf);
	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, last.size()));
	if (!data)
	{
		return	false;
	}
	std::copy(last.begin(), last.end(), data);
	return	true;
}


text root_name(text _path) noexcept
{
	text	retval(_path);
	if (retval.size() >= 2)
	{
		if (((retval[0] >= 'a' && retval[0] <= 'z') || (retval[0] >= 'A' && retval[0] <= 'Z')) && retval[1] == ':')
		{
			return	retval.substr(0, 2);
		}
	}
	return	retval.substr(0, 0);
}

AMTRS_FILESYSTEM_NAMESPACE_END
