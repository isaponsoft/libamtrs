/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__find_view_if__hpp
#define	__libamtrs__string__find_view_if__hpp
AMTRS_NAMESPACE_BEGIN

// ============================================================================
//! 
// ----------------------------------------------------------------------------
template<class CharT, class CharTraits, class Callback>
std::basic_string_view<CharT, CharTraits> find_view_if(std::basic_string_view<CharT, CharTraits> _in, Callback&& _callback)
{
	auto	cur	= _in.data();
	auto	end	= _in.data() + _in.size();
	while (cur != end)
	{
		if (auto v = _callback(std::basic_string_view<CharT, CharTraits>(cur, end - cur)); !v.empty())
		{
			return	v;
		}
		++cur;
	}
	return	_in.substr(0, 0);
}

AMTRS_NAMESPACE_END
#endif
