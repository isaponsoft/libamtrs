/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__string__starts_with__hpp
#define	__libamtrs__string__starts_with__hpp
#include <string_view>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


template<class CharT, class TraitsT = std::char_traits<CharT>>
bool starts_with(std::basic_string_view<CharT, TraitsT> _s, std::basic_string_view<CharT, TraitsT> _prefix)
{
	if (_s.size() < _prefix.size())
	{
		return	false;
	}
	return	std::equal(std::begin(_prefix), std::end(_prefix), std::begin(_s));
}


template<class CharT, class TraitsT = std::char_traits<CharT>>
bool ends_with(std::basic_string_view<CharT, TraitsT> _s, std::basic_string_view<CharT, TraitsT> _suffix)
{
	if (_s.size() < _suffix.size())
	{
		return	false;
	}
	return	std::equal(std::begin(_suffix), std::end(_suffix), std::end(_s)-_suffix.size());
}



AMTRS_NAMESPACE_END
#endif
