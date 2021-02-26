/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__psnvm__compiler__token__hpp
#define	__psnvm__compiler__token__hpp
#include ".vm-vm.hpp"
PASSION_NAMESPACE_BEGIN


inline int is_identify_first(char const* c, size_t size) noexcept
{
	if (size == 0)
	{
		return	0;
	}
	if (*c == '_' || (*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z'))
	{
		return	1;
	}
/*
	// unicode
	if (size < 3)
	{
		return	0;
	}
	if ()
	{
	}
*/
	return	0;
}

inline int is_identify_secondary(char const* c, size_t size) noexcept
{
	if (size == 0)
	{
		return	0;
	}
	if (auto s = is_identify_first(c, size); s > 0)
	{
		return	s;
	}
	if (*c >= '0' && *c <= '9')
	{
		return	1;
	}
	return	0;
}



PASSION_NAMESPACE_END
#endif
