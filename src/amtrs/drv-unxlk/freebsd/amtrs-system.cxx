/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <pwd.h>
#include <amtrs/filesystem/path.hpp>
#include <amtrs/system/env.hpp>
#include <amtrs/string/format.hpp>
#include <amtrs/string/sstring.hpp>

AMTRS_SYSTEM_NAMESPACE_BEGIN

bool get_execute_path(amtrs_bufferif_one_init _destinate, char const* _argv0)
{
	text	argv0(_argv0);
	if (filesystem::is_absolute(_argv0))
	{
		auto*		data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, argv0.size()));
		if (!data)
		{
			return	false;
		}
		std::copy_n(argv0.data(), argv0.size(), data);
		return	true;
	}

	size_t	size;
	sstring	buf;
	int		mib[4]{ CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
	int		result;

	// get buffer size
	result = sysctl(mib, sizeof(mib)/sizeof(int), nullptr, &size, nullptr, 0);
	if (result == -1)
	{
		return	_argv0;
	}
	buf.resize(size);

	// copy to buffer
	result = sysctl(mib, sizeof(mib)/sizeof(int), buf.data(), &size, nullptr, 0);
	if (result == -1)
	{
		return	_argv0;
	}

	text		dir		= filesystem::parent_path(buf);
	auto*		data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, dir.size()+1+argv0.size()));
	if (!data)
	{
		return	false;
	}
	std::copy_n(dir.data(), dir.size(), data);	data += dir.size();
	*data++ = '/';
	std::copy_n(argv0.data(), argv0.size(), data);
	return	true;
}

AMTRS_SYSTEM_NAMESPACE_END
