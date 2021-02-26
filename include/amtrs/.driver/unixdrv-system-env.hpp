/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__system__env_posix__hpp
#define	__libamtrs__system__env_posix__hpp
#include <stdio.h>
#include <stdlib.h>
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(env)

template<class S>
inline S getenv(char const* _name)
{
	auto	e = ::getenv(_name);
	return	e ? S{e} : S{};
}


AMTRS_IMPLEMENTS_END(env)
AMTRS_NAMESPACE_END
#endif
