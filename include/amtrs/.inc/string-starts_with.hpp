/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__starts_with__hpp
#define	__libamtrs__string__starts_with__hpp
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


template<class S, class M>
bool starts_with(S const& _s, M const& _m)
{
	return	starts_with((std::string_view)_s, (std::string_view)_m);
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


template<class S, class M>
bool ends_with(S const& _s, M const& _m)
{
	return	ends_with((std::string_view)_s, (std::string_view)_m);
}



AMTRS_NAMESPACE_END
#endif
