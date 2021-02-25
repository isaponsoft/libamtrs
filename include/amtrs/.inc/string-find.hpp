/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__find__hpp
#define	__libamtrs__string__find__hpp
AMTRS_NAMESPACE_BEGIN


// ============================================================================
//! _a から _b が見つかる位置を返します。
// ----------------------------------------------------------------------------
template<class CharT, class Traits>
auto find_string_first_of(std::basic_string_view<CharT, Traits> _a, std::basic_string_view<CharT, Traits> _b) -> typename std::basic_string_view<CharT, Traits>::size_type
{
	// _a が _b より短ければ即終了
	if (_a.size() < _b.size())
	{
		return	std::basic_string_view<CharT, Traits>::npos;
	}

	auto	cur		= _a.data();
	auto	tail	= _a.data() + _a.size();
	auto	end		= tail - _b.size();
	for (;;)
	{
		if (starts_with(std::basic_string_view<CharT, Traits>(cur, tail - cur), _b))
		{
			return	cur - _a.data();
		}
		if (end == cur)
		{
			return	std::basic_string_view<CharT, Traits>::npos;
		}
		++cur;
	}
}



AMTRS_NAMESPACE_END
#endif
