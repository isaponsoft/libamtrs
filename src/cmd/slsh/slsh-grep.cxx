/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
PSNVM_SLSH_NAMESPACE_BEGIN

int f_grep(sstring& e)
{
	io::fdesc::stdo() << "grp " << e << "\n";
	return	0;
}

PSNVM_SLSH_NAMESPACE_END
