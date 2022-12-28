/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <string.h>
#include <amtrs/system/process.hpp>
PSNVM_SLSH_NAMESPACE_BEGIN

int f_exec(sstring& e)
{
	auto	ext	= amtrs::system::exec(e.c_str());
	if (ext.bad())
	{
		io::fdesc::stdo() << e << ": " << ext.message() << "\n";
	}
	return	ext.code();
}

PSNVM_SLSH_NAMESPACE_END
