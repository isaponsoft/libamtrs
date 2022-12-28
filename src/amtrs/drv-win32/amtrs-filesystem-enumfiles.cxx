/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem/enumfiles.hpp>
#include <amtrs/filesystem/path.hpp>
#include <amtrs/string/convert.hpp>
#include <amtrs/string/wildcard.hpp>
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


int enumfiles(text _path, text _filter, typeutil::callback<bool(enumentry&)> _callback)
{
	sstring				path	= normalize_path<sstring>(_path).replace("/", "\\");
	if (amtrs::filesystem::is_directory(path))
	{
		path.append(sstring("\\*"));
	}

	WIN32_FIND_DATAW	fd;
	wsstring			wpath	= c8_to_wc(path);
	HANDLE				hFind	= FindFirstFileW(wpath.c_str(), &fd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return	(int)GetLastError();
	}

	wsstring			wfilter	= c8_to_wc(_filter);
    do
	{
		if (!wfilter.empty() && !wcmatch(wfilter, wtext(fd.cFileName)))
		{
			continue;
		}

		// システムかつ隠しファイルは返さない
		if (fd.dwFileAttributes == (FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_ARCHIVE))
		{
			continue;
		}

		enumentry	st;
		st.name	= wc_to_c8(fd.cFileName);
		st.type	= 0;
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			st.type	|= enumentry::fst_directory;
		}
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
		{
			st.type	|= enumentry::fst_hidden;
		}
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
		{
			st.type	|= enumentry::fst_symlink;
		}
		if (amtrs::filesystem::extension(st.name) == ".exe" || amtrs::filesystem::extension(st.name) == ".com" || amtrs::filesystem::extension(st.name) == ".bat")
		{
			st.type	|= enumentry::fst_execute;
		}
		if (!_callback(st))
		{
			break;
		}
    } while(FindNextFileW(hFind, &fd));
	DWORD	err	= GetLastError();
	return	err == ERROR_NO_MORE_FILES ? 0 : (int)err;
}


AMTRS_FILESYSTEM_NAMESPACE_END
