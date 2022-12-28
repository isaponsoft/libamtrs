/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <algorithm>
#include <stdexcept>
#include <amtrs/string/char_iterator.hpp>
#include <amtrs/string/wildcard.hpp>
AMTRS_NAMESPACE_BEGIN
enum class result_code
{
	nomatch,
	matching,
	patturnerror,
};

template<class CharT>
struct	group
{
	using	match_type	= basic_wildcard<CharT>;
	using	group_index	= typename match_type::group_index;
	group_index	prev;
	match_type	mg;
};

template<class CharT, class Iterator>
struct	matcher
{
	using	match_type	= basic_wildcard<CharT>;
	using	iterator	= Iterator;
	using	char_type	= CharT;
	using	int_type	= typename iterator::int_type;
	using	view_type	= typename match_type::view_type;
	using	group_index	= typename match_type::group_index;

	simplearray<group<CharT>>	groups;
	group_index					index;
	iterator					ite;
	char_type const*			string;
	char_type const*			patturn;
	char_type const*			string_end;
	char_type const*			patturn_end;
	bool						cased;

	static inline int_type lower_case(int_type _code) noexcept
	{
		return	(_code >= 'A' && _code <= 'Z') ? (_code - 'A' + 'a') :  _code;
	}

	static inline int_type upper_case(int_type _code) noexcept
	{
		return	(_code >= 'a' && _code <= 'z') ? (_code - 'a' + 'A') :  _code;
	}

	inline int_type read_patturn() noexcept
	{
		int_type	c;
		patturn	= ite.read(c, patturn, patturn_end);
		if (c == '\\' && !ite.empty(patturn, patturn_end))
		{
			patturn	= ite.read(c, patturn, patturn_end);
			     if (c == 't' ) { c = '\t'; }
			else if (c == 'n' ) { c = '\n'; }
			else if (c == 'r' ) { c = '\r'; }
		}
		return	c;
	}

	inline int_type read_string() noexcept
	{
		int_type	c;
		string	= ite.read(c, string, string_end);
		return	c;
	}

	result_code match() noexcept;
};


template<class CharT, class Iterator>
auto matcher<CharT, Iterator>::match() noexcept -> result_code
{
	// パターンが終わるまで繰り返す
	while (patturn < patturn_end)
	{
		int_type	c	= read_patturn();
		if (c == 0)
		{
			return	result_code::patturnerror;
		}
		switch (c)
		{
			case '(' :
			{
				group<CharT>	g;
				g.prev			= index;
				g.mg.index		= static_cast<group_index>(groups.size());
				g.mg.match		= { string   , static_cast<size_t>(0) };
				g.mg.patturn	= { patturn-1, static_cast<size_t>(0) };
				groups.push_back(std::move(g));
				index	= groups.back().mg.index;
				break;
			}
			case ')' :
			{
				if (index == -1)
				{
					return	result_code::patturnerror;
				}
				group<CharT>&	g	= groups[index];
				g.mg.match			= { g.mg.match.data()  , static_cast<size_t>(string - g.mg.match.data()) };
				g.mg.patturn		= { g.mg.patturn.data(), static_cast<size_t>(patturn - g.mg.patturn.data()) };
				index				= g.prev;
				break;
			}
			case '*' :
			{
				// 連続する '*' をスキップする
				for (;;)
				{
					// 最後まで '*' なら全体にマッチしたものとして扱う
					if (patturn == patturn_end)
					{
						return	result_code::matching;
					}
					if (*patturn != '*')
					{
						break;
					}
					++patturn;
				}
				// 現在のグループの状態と検索位置を保存
				auto		groups_size		= groups.size();
				auto*		patturn_current	= patturn;
				auto		index_current	= index;
				for (;;)
				{
					auto*	string_current	= string;
					auto	res				= match();
					if (res != result_code::nomatch)
					{
						return	res;
					}
					if (string_current == string_end)
					{
						return	result_code::nomatch;
					}
					groups.resize(groups_size);
					string	= string_current + 1;
					patturn	= patturn_current;
					index	= index_current;
				}
			}
			case '+' :
			{
				// '+' をスキップする
				for (;;)
				{
					// 最後まで '+' なら終了
					if (patturn == patturn_end)
					{
						return	result_code::matching;
					}
					if (*patturn != '+')
					{
						break;
					}
					++patturn;
				}
				auto	groups_size		= groups.size();
				auto*	patturn_current	= patturn;
				auto	index_current	= index;
				auto*	string_begin	= string;
				string	= string_end;
				for (;;)
				{
					auto*	string_current	= string;
					auto	res				= match();
					if (res != result_code::nomatch)
					{
						return	res;
					}
					if (string_current == string_begin)
					{
						return	result_code::nomatch;
					}
					groups.resize(groups_size);
					string	= string_current-1;
					patturn	= patturn_current;
					index	= index_current;
				}
			}
			// '?' ならどの文字とも一致させる
			case '?' :
			{
				// 文字がない場合は一致しない扱い
				if (string == string_end)
				{
					return	result_code::nomatch;
				}
				++string;
				break;
			}
			// '[' パターン一致
			case '[' :
			{
				bool	isNot	= false;
				bool	isMatch	= false;		// １文字でもマッチすれば true
				auto*	start	= patturn-1;
				auto	s		= read_string();
				if (s == 0)
				{
					return	result_code::patturnerror;
				}
				// '^' から始まれば否定
				if (patturn < patturn_end && *patturn == '^')
				{
					isNot	= true;
					++patturn;
				}
				// ']' が出るまで
				for (;;)
				{
					// パターン終了
					if (patturn == patturn_end)
					{
						return	result_code::patturnerror;
					}
					if ((c = read_patturn()) == ']')
					{
						break;
					}
					if (c == 0)
					{
						return	result_code::patturnerror;
					}
					// エスケープコード
					if (c == '\\')
					{
						if (patturn == patturn_end)
						{
							return	result_code::patturnerror;
						}
						// 文字が一致すれば抜ける
						c = read_patturn();
						if (c == 0)
						{
							return	result_code::patturnerror;
						}
						if (s == c)
						{
							isMatch	= true;
						}
						continue;
					}
					// 次の文字が '-' の場合
					if (*patturn == '-')
					{
						// 次の文字が '-' なのに終端文字とかない
						if ((patturn+2) >= patturn_end)
						{
							return	result_code::patturnerror;
						}
						++patturn;
						auto	current	= c;
						auto	last	= read_patturn();
						if (last == 0)
						{
							return	result_code::patturnerror;
						}
						if (current > last)
						{
							last		= current;
							current		= last;
						}
						if (!cased)
						{
							current	= lower_case(current);
							last	= lower_case(last);
							s		= lower_case(s);
						}
						if (s >= current && s <= last)
						{
							isMatch	= true;
						}
						continue;
					}
					// それ以外の文字
					if (!cased)
					{
						// 大文字小文字関係なしに一致するかどうか
						if (lower_case(c) == lower_case(s))
						{
							isMatch	= true;
						}
					}
					else
					{
						// 一致するかどうか
						if (c == s)
						{
							isMatch	= true;
						}
					}
				}
				// 一致しない場合は非一致を返す
				if (isNot ? (isMatch) : (!isMatch))
				{
					return	result_code::nomatch;
				}
				break;
			}
			// クラスの終了
			case ']' :
			{
				//patturn_error	= patturn-1;
				return	result_code::patturnerror;
			}
			// そのほかの文字は一致するか調べる
			default :
			{
				auto	s	= read_string();
				if (s == 0)
				{
					return	result_code::nomatch;
				}
				if (!cased)
				{
					// 大文字小文字関係なしに一致するかどうか
					if (lower_case(c) != lower_case(s))
					{
						// 一致しない
						return	result_code::nomatch;
					}
				}
				else
				{
					// 一致するかどうか
					if (c != s)
					{
						// 一致しない
						return	result_code::nomatch;
					}
				}
			}
		}
	}
	if (string == string_end)
	{
		return	result_code::matching;
	}
	return	result_code::nomatch;
}

template<class CharT, class Iterator>
size_t basic_match(basic_text<CharT> _patturn, basic_text<CharT> _string, bool _cased)
{
	using	match_type				= basic_wildcard<CharT>;
	using	iterator				= Iterator;
	using	char_type				= CharT;
	using	int_type				= typename iterator::int_type;
	using	view_type				= typename match_type::view_type;
	using	group_inde				= typename match_type::group_index;


	matcher<CharT, Iterator>		m;
	m.index			= -1;
	m.patturn		= _patturn.data();
	m.patturn_end	= _patturn.data() + _patturn.size();
	m.string		= _string.data();
	m.string_end	= _string.data() + _string.size();
	m.cased			= _cased;

	auto	res	= m.match();
	if (res == result_code::patturnerror)
	{
		throw	std::logic_error("wildcard regex error.");
	}
	if (res == result_code::nomatch)
	{
		return	0;
	}
	return	(size_t)(1 + m.groups.size());
}


template<class CharT, class Iterator, class Callback>
size_t basic_match(basic_text<CharT> _patturn, basic_text<CharT> _string, bool _cased, Callback&& _callback)
{
	using	match_type				= basic_wildcard<CharT>;
	using	iterator				= Iterator;
	using	char_type				= typename std::remove_const<CharT>::type;
	using	int_type				= typename iterator::int_type;
	using	view_type				= typename match_type::view_type;
	using	group_inde				= typename match_type::group_index;
	using	callback_result_type	= decltype(_callback(std::declval<match_type&>()));


	matcher<CharT, Iterator>		m;
	m.index			= -1;
	m.patturn		= _patturn.data();
	m.patturn_end	= _patturn.data() + _patturn.size();
	m.string		= _string.data();
	m.string_end	= _string.data() + _string.size();
	m.cased			= _cased;

	auto	res	= m.match();
	if (res == result_code::patturnerror)
	{
		throw	std::logic_error("wildcard regex error.");
	}
	if (res == result_code::nomatch)
	{
		return	0;
	}

	match_type	all	= {
		0,
		_string,
		_patturn
	};

	bool	loop	= true;
	if (std::is_same<void, callback_result_type>::value)
	{
		_callback(all);
	}
	else
	{
		loop	= _callback(all);
	}
	if (loop)
	{
		for (auto& g : m.groups)
		{
			++g.mg.index;
			if (std::is_same<void, callback_result_type>::value)
			{
				_callback(g.mg);
			}
			else
			{
				if (!_callback(g.mg))
				{
					break;
				}
			}
		}
	}
	return	(size_t)(1 + m.groups.size());
}


size_t wcmatch   (wildcard* _results, text _patturn, text _string, bool _cased)
{
	int		i	= -1;
	return	basic_match<char, ascii_iterator<char>>(_patturn, _string, _cased, [&](auto& m) -> bool
			{
				if (i >= 0)
				{
					_results[i] = m;
				}
				++i;
				return	true;
			});
}

size_t wcmatch   (wwildcard* _results, wtext _patturn, wtext _string, bool _cased)
{
	int		i	= -1;
	return	basic_match<wchar_t, ascii_iterator<wchar_t>>(_patturn, _string, _cased, [&](auto& m) -> bool
			{
				if (i >= 0)
				{
					_results[i] = m;
				}
				++i;
				return	true;
			});
}

size_t wcmatch_mb(wildcard* _results, text _patturn, text _string, bool _cased)
{
	int		i	= -1;
	return	basic_match<char, mb_iterator<char>>(_patturn, _string, _cased, [&](auto& m) -> bool
			{
				if (i >= 0)
				{
					_results[i] = m;
				}
				++i;
				return	true;
			});
}


size_t wcmatch(text _patturn, text _string)
{
	return	basic_match<char, ascii_iterator<char>>(_patturn, _string, true);
}

size_t wcmatch(wtext _patturn, wtext _string)
{
	return	basic_match<wchar_t, ascii_iterator<wchar_t>>(_patturn, _string, true);
}

size_t wcmatch(text _patturn, text _string, bool _cased)
{
	return	basic_match<char, ascii_iterator<char>>(_patturn, _string, _cased);
}

size_t wcmatch(wtext _patturn, wtext _string, bool _cased)
{
	return	basic_match<wchar_t, ascii_iterator<wchar_t>>(_patturn, _string, _cased);
}

size_t wcmatch(text _patturn, text _string, bool _cased, bool(*_callback)(void*,wildcard&), void* _userval)
{
	return	basic_match<char, ascii_iterator<char>>(_patturn, _string, _cased, [&](wildcard& _wc) -> bool
			{
				return	_callback(_userval, _wc);
			});
}

size_t wcmatch(wtext _patturn, wtext _string, bool _cased, bool(*_callback)(void*,wwildcard&), void* _userval)
{
	return	basic_match<wchar_t, ascii_iterator<wchar_t>>(_patturn, _string, _cased, [&](wwildcard& _wc) -> bool
			{
				return	_callback(_userval, _wc);
			});
}


size_t wcmatch_mb(text _patturn, text _string)
{
	return	basic_match<char, mb_iterator<char>>(_patturn, _string, true);
}

size_t wcmatch_mb(text _patturn, text _string, bool _cased)
{
	return	basic_match<char, mb_iterator<char>>(_patturn, _string, _cased);
}

size_t wcmatch_mb(text _patturn, text _string, bool _cased, bool(*_callback)(void*,wildcard&), void* _userval)
{
	return	basic_match<char, mb_iterator<char>>(_patturn, _string, _cased, [&](wildcard& _wc) -> bool
			{
				return	_callback(_userval, _wc);
			});
}

AMTRS_NAMESPACE_END
