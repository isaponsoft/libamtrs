/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#if	__has_include(<fts.h>)
#include <fts.h>
#endif

#include <amtrs/filesystem/enumfiles.hpp>
#include <amtrs/string/wildcard.hpp>
#include "system.hpp"

AMTRS_FILESYSTEM_NAMESPACE_BEGIN

int enumfiles(text _dir, text _filter, typeutil::callback<bool(enumentry&)> _callback)
{
	sstring			path	= absolute_path<sstring>(_dir);
	if (path.empty())
	{
		return	errno;
	}
	bool			wsl		= system::is_windows_subsystem();
	char*			names[]	= { (char*)path.c_str(), nullptr };
	int				flags	= FTS_COMFOLLOW|FTS_PHYSICAL|FTS_NOCHDIR|FTS_SEEDOT|FTS_XDEV;
	if (wsl)
	{
		flags |= FTS_NOSTAT;
	}
	FTS*			fts 	= fts_open(names, flags, nullptr);
    if (!fts)
	{
		return	errno;
    }

	int			r		= 0;
	uid_t		uid		= geteuid();
	gid_t		gid		= getegid();
	FTSENT*		ent		= fts_read(fts);
	if (!ent)
	{
		int		r	= errno;
		fts_close(fts);
		return	r;
	}

	FTSENT*	node	= fts_children(fts, 0);
	if (!node)
	{
		int		r	= errno;
		fts_close(fts);
		return	r;
	}

	while (node)
	{
		if (!_filter.empty() && !wcmatch(_filter, text(node->fts_name, (size_t)node->fts_namelen)))
		{
			node = node->fts_link;
			continue;
		}

		enumentry	st;
		st.name	= sstring(node->fts_name, (size_t)node->fts_namelen);
		st.type	= 0;

		if (node->fts_info == FTS_D || node->fts_info == FTS_DOT)
		{
			st.type	|= enumentry::fst_directory;
		}
		if (node->fts_info == FTS_SL)
		{
			st.type	|= enumentry::fst_symlink;
		}
		if (node->fts_name[0] == '.')
		{
			st.type	|= enumentry::fst_hidden;
		}

		if (!wsl)
		{
			if ((uid == node->fts_statp->st_uid && node->fts_statp->st_mode & 0100)
			 || (gid == node->fts_statp->st_gid && node->fts_statp->st_mode & 010)
			 || (                                  node->fts_statp->st_mode & 01))
			{
				st.type	|= enumentry::fst_execute;
			}
		}
		else
		{
			struct	::stat	s;
			sstring	fullname	= path;
			fullname.append("/");
			fullname.append(text(node->fts_name, (size_t)node->fts_namelen));
			stat(fullname.c_str(), &s);
			if ((uid == s.st_uid && s.st_mode & 0100)
			 || (gid == s.st_gid && s.st_mode & 010)
			 || (                   s.st_mode & 01))
			{
				st.type	|= enumentry::fst_execute;
			}
		}
		if (!_callback(st))
		{
			break;
		}
		node = node->fts_link;
	}
	fts_close(fts);
	return	0;
}

AMTRS_FILESYSTEM_NAMESPACE_END
