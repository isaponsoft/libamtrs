/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem/path.hpp>
#include <amtrs/system/env.hpp>
PSNVM_SLSH_NAMESPACE_BEGIN

int f_cd(sstring& e)
{
	if (e.empty() || e == "~")
	{
		e	= amtrs::system::homedir<sstring>();
    }
	if (filesystem::current_path(e))
	{
		return	0;
	}
	io::fdesc::stdo() << "No such directory.\n";
	return	0;
}


PSNVM_SLSH_NAMESPACE_END
