/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <dirent.h>
#include <fts.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <amtrs/filesystem/enumfiles.hpp>
#include <amtrs/string/wildcard.hpp>
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

int enumfiles(text _path, text _filter, typeutil::callback<bool(enumentry&)> _callback)
{
	DIR*	dir	= opendir(sstring(_path).c_str());
	if (!dir)
	{
		return	errno;
	}

	struct dirent*	entry;
	uid_t			uid		= geteuid();
	gid_t			gid		= getegid();
	while ((entry = readdir(dir)) != nullptr)
	{
		if (!_filter.empty() && !wcmatch(_filter, text(entry->d_name)))
		{
			continue;
		}

		struct	::stat	s;
		sstring	fullname	= _path;
		fullname.append("/");
		fullname.append(entry->d_name);
		stat(fullname.c_str(), &s);

		enumentry	st;
		st.name	= sstring(entry->d_name);
		st.type	= 0;
		if (entry->d_type & DT_DIR)
		{
			st.type	|= enumentry::fst_directory;
		}
		if (entry->d_type & DT_LNK)
		{
			st.type	|= enumentry::fst_symlink;
		}
		if (entry->d_name[0] == '.')
		{
			st.type	|= enumentry::fst_hidden;
		}
		if ((uid == s.st_uid && s.st_mode & 0100)
		 || (gid == s.st_gid && s.st_mode & 010)
		 || (                   s.st_mode & 01))
		{
			st.type	|= enumentry::fst_execute;
		}

		if (!_callback(st))
		{
			break;
		}
	}
	int	r	= errno;
    closedir(dir);
	return	r;
}

AMTRS_FILESYSTEM_NAMESPACE_END
