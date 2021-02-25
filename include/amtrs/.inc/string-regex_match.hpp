/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__regex__regex_match__hpp
#define	__libamtrs__string__regex__regex_match__hpp
AMTRS_NAMESPACE_BEGIN



template<class It, class CharT, class Traits, class Callback>
size_t regex_match_callback(It _first, It _last,
    std::basic_regex<CharT,Traits> const & _match, Callback&& _callback)
{
	std::basic_string<CharT>	retval;
	size_t						mc		= 0;
	It							pos		= _first;
	std::for_each(
		std::regex_iterator<It>(_first, _last, _match),
		std::regex_iterator<It>(),
		[&](auto const& match)
	{
		++mc;
		_callback(match);
	});
	return	mc;
}


template<class CharT, class Traits = std::char_traits<CharT>, class RegexTraits = std::regex_traits<CharT>, class Callback>
size_t regex_match_callback(std::basic_string<CharT, Traits> const& s, const std::basic_regex<CharT, RegexTraits>& _match, Callback&& _callback)
{
	return	regex_match_callback(s.begin(), s.end(), _match, std::forward<Callback>(_callback));
}


template<class Str, class Reg, class Callback>
size_t regex_match_callback(Str const& s, Reg const& _match, Callback&& _callback)
{
	return	regex_match_callback(std::begin(s), std::end(s), std::regex(_match), std::forward<Callback>(_callback));
}


AMTRS_NAMESPACE_END
#endif
