/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__regex__regex_replace__hpp
#define	__libamtrs__string__regex__regex_replace__hpp
AMTRS_NAMESPACE_BEGIN



template<class It, class CharT, class Traits, class Callback>
std::basic_string<CharT> regex_replace_callback(It _first, It _last,
    const std::basic_regex<CharT,Traits>& _match, Callback&& _callback)
{
	std::basic_string<CharT>	retval;
	It							pos		= _first;
	std::for_each(
		std::regex_iterator<It>(_first, _last, _match),
		std::regex_iterator<It>(),
		[&](auto const& match)
	{
		auto	mpos	= match.position(0);
		auto	msize	= match.length(0);
		retval.append(pos, _first + mpos);
		retval.append(_callback(match));
		pos		= _first + mpos + msize;
	});
	retval.append(pos, _last);
	return	retval;
}


template<class CharT, class Traits = std::char_traits<CharT>, class RegexTraits = std::regex_traits<CharT>, class Callback>
auto regex_replace_callback(std::basic_string<CharT, Traits> const& s, const std::basic_regex<CharT, RegexTraits>& _match, Callback&& _callback)
	-> std::basic_string<CharT, Traits>
{
	return	regex_replace_callback(s.begin(), s.end(), _match, std::forward<Callback>(_callback));
}


template<class Str, class Reg, class Callback>
auto regex_replace_callback(Str const& s, Reg const& _match, Callback&& _callback)
{
	return	regex_replace_callback(std::begin(s), std::end(s), std::regex(_match), std::forward<Callback>(_callback));
}


AMTRS_NAMESPACE_END
#endif
