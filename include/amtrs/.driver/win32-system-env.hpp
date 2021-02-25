/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__system__env_win32__hpp
#define	__libamtrs__system__env_win32__hpp
#include <vector>
#include <stdio.h> 
#include "env.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(env)

template<class S>
inline S getenv(char const* _name)
{
	std::vector<char>	buff;

	size_t	buffSize;
	getenv_s(&buffSize, nullptr, 0, _name);
	if (buffSize == 0)
	{
		return	{};
	}

	buff.resize(buffSize);
	getenv_s(&buffSize, buff.data(), buff.size(), _name);

	return	{buff.data(), buff.size() - 1};
}


AMTRS_IMPLEMENTS_END(env)
AMTRS_NAMESPACE_END
#endif
