/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__trim__hpp
#define	__libamtrs__string__trim__hpp
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(trim)
template<class CharT, class TraitsT>
bool has(CharT _c, std::basic_string_view<CharT, TraitsT> _delms)
{
	for (auto d : _delms)
	{
		if (d == _c)
		{
			return	true;
		}
	}
	return	false;
}

AMTRS_IMPLEMENTS_END(trim)


template<class CharT, class TraitsT>
std::basic_string_view<CharT, TraitsT> trim(std::basic_string_view<CharT, TraitsT> _string, std::basic_string_view<CharT, TraitsT> _delms)
{
	if (_string.empty())
	{
		return	_string;
	}

	auto	begin	= _string.begin();
	auto	end		= _string.end();
	while (begin != end && AMTRS_IMPLEMENTS(trim)::has(*begin, _delms))
	{
		++begin;
	}
	if (begin != end)
	{
		while (begin != (end-1))
		{
			if (!AMTRS_IMPLEMENTS(trim)::has(*(end-1), _delms))
			{
				break;
			}
			--end;
		}
	}
	if (end == begin)
	{
		return	{};
	}
	return	{ &*begin, typename std::basic_string_view<CharT, TraitsT>::size_type(end - begin) };
}


template<class S, class Dem>
auto trim(S const& _string, Dem const& _dem)
{
	return	trim((std::string_view)_string, (std::string_view)_dem);
}


AMTRS_NAMESPACE_END
#endif
