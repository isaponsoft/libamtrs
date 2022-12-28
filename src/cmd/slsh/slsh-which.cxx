/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem/enumfiles.hpp>
#include <amtrs/string/split_iterator.hpp>
#include <amtrs/system/env.hpp>
PSNVM_SLSH_NAMESPACE_BEGIN

int f_which(sstring& e)
{
	auto	env		= system::getenv<sstring>("PATH");
	auto	paths	= split_chars(env, system::env_separator());
	for (auto findpath : paths)
	{
		bool	exit = false;
		filesystem::enumfiles(findpath, {}, [&](filesystem::enumentry& stat) -> bool
		{
			if (stat.name == e)
			{
				io::fdesc::stdo() << findpath << "/" << e << "\n";
				exit	= true;
				return	false;
			}
			return	true;
		});
		if (exit)
		{
			break;
		}
	}
	return	0;
}

PSNVM_SLSH_NAMESPACE_END
