/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__string__trim__hpp
#define	__libamtrs__string__trim__hpp
#include <string_view>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(trim)
template<class CharT>
bool has(CharT _c, const CharT* _delms)
{
	while (*_delms)
	{
		if (*_delms++ == _c)
		{
			return	true;
		}
	}
	return	false;
}

AMTRS_IMPLEMENTS_END(trim)


template<class CharT, class TraitsT>
std::basic_string_view<CharT, TraitsT> trim(std::basic_string_view<CharT, TraitsT> _string, const CharT* _delms)
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



AMTRS_NAMESPACE_END
#endif
