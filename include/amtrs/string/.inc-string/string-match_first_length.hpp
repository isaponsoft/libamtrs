/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__match_first_length__hpp
#define	__libamtrs__string__match_first_length__hpp
AMTRS_NAMESPACE_BEGIN


/*
 * 先頭から調べて一致している長さを返す。
 */
template<class A, class B>
size_t match_first_length(A&& _a, B&& _b)
{
	std::string_view	a(_a);
	std::string_view	b(_b);
	size_t				l	= std::min<size_t>(a.size(), b.size());
	size_t				s;
	for (s = 0; s < l; ++s)
	{
		if (a[s] != b[s])
		{
			break;
		}
	}
	return	s;
}


AMTRS_NAMESPACE_END
#endif
