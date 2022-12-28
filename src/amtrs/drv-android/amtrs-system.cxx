/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <pwd.h>
#include <amtrs/system/env.hpp>
#include "system.hpp"

AMTRS_SYSTEM_NAMESPACE_BEGIN

static bool is_sep(char c) { return c == '\\' || c == '/'; }
static int	gSystemTypeWSL	= -1;

bool is_windows_subsystem()
{
	if (gSystemTypeWSL == -1)
	{
		gSystemTypeWSL	= 0;
		if (!getenv<sstring>("WSL_DISTRO_NAME").empty())
		{
			gSystemTypeWSL	= 1;
		}
	}
	return	gSystemTypeWSL == 1;
}


bool getenv(amtrs_bufferif_one_init _destinate, text _name)
{
	sstring		name	= _name;
	auto		e 		= text(::getenv(name.c_str()));
	auto*		data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, e.size()));
	if (!data)
	{
		return	false;
	}
	std::copy_n(e.data(), e.size(), data);
	return	true;
}

text env_separator()
{
	return	":";
}


void setenv(text _key, text _value)
{
	sstring	k(_key);
	sstring	v(_value);
	::setenv(k.c_str(), v.c_str(), 1);
}


static bool make(amtrs_bufferif_one_init _alloc, text _dir)
{
	auto*		data	= reinterpret_cast<char*>(_alloc.allocate(_alloc.object, _dir.size()));
	if (!data)
	{
		return	false;
	}
	std::copy_n(_dir.data(), _dir.size(), data);
	return	true;
}



//! プラットフォームの特殊なファイルパスを取得します。
bool special_path(amtrs_bufferif_one_init _destinate, specialpathtype _type, text _appname)
{
	// https://specifications.freedesktop.org/basedir-spec/latest/ar01s03.html
	if (_type == specialpathtype::home)
	{
		if (getenv(_destinate, text("HOME")))
		{
			return	true;
		}
	    struct passwd*	pw	= getpwuid(getuid());
		if (pw)
		{
			return	make(_destinate, pw->pw_dir);
		}
		return	false;
	}
	else
	{
		sstring	home	= getenv<sstring>("XDG_DATA_HOME");
		if (home.empty())
		{
			home	= getenv<sstring>("HOME");
		}
		if (_type == specialpathtype::cache)
		{
			sstring	cacheDir	= getenv<sstring>("XDG_CACHE_HOME");
			if (cacheDir.empty())
			{
				cacheDir	= home + sstring("/.cache/") + sstring(_appname);
			}
			return	make(_destinate, cacheDir);
		}
		if (_type == specialpathtype::config)
		{
			sstring	cacheDir	= getenv<sstring>("XDG_CONFIG_HOME");
			if (cacheDir.empty())
			{
				cacheDir	= home + sstring("/.cache/") + sstring(_appname);
			}
			return	make(_destinate, cacheDir);
		}
		if (_type == specialpathtype::app_local || _type == specialpathtype::app_backup)
		{
			sstring	localDir	= getenv<sstring>("XDG_DATA_HOME");
			if (localDir.empty())
			{
				localDir	= home + sstring("/.local/share/") + sstring(_appname);
			}
			return	make(_destinate, localDir);
		}
	}
	return	false;
}


bool hostname(amtrs_bufferif_one_init _destinate)
{
	struct utsname	un;
	uname(&un);
	return	make(_destinate, un.nodename);
}


bool username(amtrs_bufferif_one_init _destinate)
{
	auto	u	= getlogin();
	if (u)
	{
		return	make(_destinate, u);
	}
    struct passwd*	pw	= getpwuid(getuid());
	if (pw)
	{
		return	make(_destinate, pw->pw_name);
	}
	return	false;
}

AMTRS_SYSTEM_NAMESPACE_END
