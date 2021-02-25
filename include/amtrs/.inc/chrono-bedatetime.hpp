/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__bedatetime__hpp
#define	__libamtrs__bedatetime__hpp
AMTRS_CHRONO_NAMESPACE_BEGIN



// ============================================================================
//! BigEndian Date Time Format.
// ----------------------------------------------------------------------------
//! Y[Y*]/MM/DD hh:mm:ss[ ][+-HH:MM]
//! 結果を gmtime で返します。
// ----------------------------------------------------------------------------
template<class Char, class Traits>
auto bedatetime(std::time_t* _ret, std::basic_string_view<Char, Traits> _string) -> std::basic_string_view<Char, Traits>
{
	auto	__parse_int	= [](auto& _val, std::basic_string_view<Char, Traits> _string, size_t _len) -> std::basic_string_view<Char, Traits>
	{
		auto const*	cur		= _string.data();
		auto const*	end		= cur + (_len ? std::min(_len, _string.size()) : _string.size());
		_val	= 0;
		while (cur != end && (*cur >= '0' && *cur <= '9'))
		{
			_val = _val * 10 + (*cur - '0');
			++cur;
		}
		return	std::basic_string_view<Char, Traits>(cur, _string.data()+_string.size()-cur);
	};


	std::tm		tm	= {};
	auto		str	= _string;

	// year
	str	= __parse_int(tm.tm_year, str, 0);
	if (str.empty() || str[0] != '/')
	{
		return	_string;
	}
	str		= str.substr(1);
	tm.tm_year	-= 1900;

	// month
	str	= __parse_int(tm.tm_mon, str, 2);
	tm.tm_mon	-= 1;
	if (str.empty() || str[0] != '/')
	{
		return	_string;
	}
	str		= str.substr(1);

	// mday
	str	= __parse_int(tm.tm_mday, str, 2);
	if (str.empty() || str[0] != ' ')
	{
		return	_string;
	}
	str		= str.substr(1);

	// hour
	str	= __parse_int(tm.tm_hour, str, 2);
	if (str.empty() || str[0] != ':')
	{
		return	_string;
	}
	str		= str.substr(1);

	// min
	str	= __parse_int(tm.tm_min, str, 2);
	if (str.empty() || str[0] != ':')
	{
		return	_string;
	}
	str		= str.substr(1);

	// sec
	str	= __parse_int(tm.tm_sec, str, 2);

	// エポックタイムからの経過間に変換しておく
	std::time_t	retval	= mktime(&tm);

	// タイムゾーンのオフセット値を計算
	if (!str.empty() && (str[0] == '+' || str[0] == '-' || str[0] == ' '))
	{
		auto	pos	= str;
		if (str[0] == ' ')
		{
			str		= str.substr(1);
			if (str.empty() || (str[0] != '+' && str[0] != '-'))
			{
				*_ret	= retval;
				return	pos;
			}
		}

		bool	minus	= str[0] == '-';
		int		zh		= 0;
		int		zm		= 0;
		str	=  __parse_int(zh, str.substr(1), 2);
		retval	+= (minus ? -zh : zh) * (60*60);
		if (!str.empty() && str[0] == ':')
		{
			str		=  str.substr(1);
			str		=  __parse_int(zm, str, 2);
			retval		+= (minus ? -zm : zm) * 60;
		}
	}

	*_ret	= retval;

	return	str;
}




AMTRS_CHRONO_NAMESPACE_END
#endif

