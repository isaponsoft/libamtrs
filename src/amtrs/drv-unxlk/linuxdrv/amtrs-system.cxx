/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <stdio.h>
#include <stdlib.h>
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
	simplearray<char>	buf;
	buf.resize(512);
	size_t				size	= buf.size();
	for (;;)
	{
		auto	result = readlink("/proc/self/exe", buf.data(), size);
	    if (result < 0)
	    {
			return	false;
	    }
	    else if (static_cast<size_t>(result) < buf.size())
	    {
			break;
	    }
		buf.resize(buf.size() * 2);
		size	= buf.size();
	}
	auto*		data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, size));
	if (!data)
	{
		return	false;
	}
	std::copy_n(buf.data(), size, data);
	return	true;
}

AMTRS_SYSTEM_NAMESPACE_END
