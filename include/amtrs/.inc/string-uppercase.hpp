/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__uppercase__hpp
#define	__libamtrs__string__uppercase__hpp
AMTRS_NAMESPACE_BEGIN


template<class T, class CharT, class Traits>
auto uppercase(std::basic_string_view<CharT, Traits> _in) -> T
{
	T		retval;
	retval.resize(_in.size());
	auto	d	= retval.data();
	auto	s	= _in.data();
	for (size_t i = 0; i < _in.size(); ++i)
	{
		d[i]	= (CharT)std::toupper(s[i]);
	}
	return	retval;
}


AMTRS_NAMESPACE_END
#endif
