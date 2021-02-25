/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__wildcard__hpp
#define	__libamtrs__string__wildcard__hpp
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(wildcard)
enum class result_code
{
	nomatch,
	matching,
	patturnerror,
};
AMTRS_IMPLEMENTS_END(wildcard)

template<class CharT, class Traits = std::char_traits<CharT>>
struct	basic_wildcard
{
	using	view_type	= std::basic_string_view<CharT, Traits>;
	using	group_index	= int;

	group_index		index;	
	view_type		match;
	view_type		patturn;
};

using	wildcard	= basic_wildcard<char>;
using	wwildcard	= basic_wildcard<wchar_t>;

/*
 * ワイルドカードによる文字列の比較を行います。
 * 正規表現に比べてできることは多くありませんがその分軽量です。
 * ファイル名検索など簡単な比較に向いています。
 *
 *	_callback : void(wildcard&)
 *		コールバック関数を指定した場合、最終的にマッチしたグループの
 *		情報をコールバック関数に渡します。
 *		どのグループがどこの部分にマッチしたのかを知る事が出来ます。
 *
 *	patturn syntax
 *		()		: グループ化。コールバック関数で調べることが出来ます。
 *		*		: ０文字以上の任意の文字（最短一致）
 *		+		: ０文字以上の任意の文字（最長一致）
 *		?		: １文字の任意の文字
 *		[a-z]	: a から z までの文字
 *		[^ABC]	: ABC以外の文字
 *
 *	またパターン中二は以下のエスケープが使用できます。
 *		\t		: tab
 *		\r		:
 *		\n		:
 *		\\		: \
 */
template<class Callback, class CharT, class Traits = std::char_traits<CharT>>
size_t basic_match(std::basic_string_view<CharT, Traits> _string, std::basic_string_view<CharT, Traits> _patturn, bool _cased, Callback&& _callback)
{
	using namespace	AMTRS_IMPLEMENTS(wildcard);

	using	match_type	= basic_wildcard<CharT, Traits>;
	using	char_type	= typename std::remove_const<CharT>::type;
	using	view_type	= typename match_type::view_type;
	using	group_index	= typename match_type::group_index;
	struct	group
	{
		group_index	prev;
		match_type	mg;
	};
	struct	_
	{
		std::vector<group>	groups;
		group_index			index;
		char_type const*	string;
		char_type const*	patturn;
		char_type const*	string_end;
		char_type const*	patturn_end;
		bool				cased;

		static inline char_type lower_case(char_type _code) noexcept
		{
			return	(_code >= 'A' && _code <= 'Z') ? (_code - 'A' + 'a') :  _code;
		}

		static inline char_type upper_case(char_type _code) noexcept
		{
			return	(_code >= 'a' && _code <= 'z') ? (_code - 'a' + 'A') :  _code;
		}

		inline char_type read_patturn() noexcept
		{
			char_type	c	= (patturn < patturn_end)
							? *patturn++
							: 0;
			if (c == '\\' && patturn < patturn_end)
			{
				c	= *patturn++;
				if (c == 't') { c = '\t'; }
				if (c == 'n') { c = '\n'; }
				if (c == 'r') { c = '\r'; }
			}
			return	c;
		}

		inline char_type read_string() noexcept
		{
			return	(string < string_end)
					? *string++
					: 0;
		}

		result_code match() noexcept
		{
			// パターンが終わるまで繰り返す
			while (patturn < patturn_end)
			{
				char_type	c	= read_patturn();
				if (c == 0)
				{
					return	result_code::patturnerror;
				}
				switch (c)
				{
					case '(' :
					{
						groups.push_back(group{
							.prev			= index,
							.mg				= match_type {
								.index		= static_cast<group_index>(groups.size()),
								.match		= { string   , 0 },
								.patturn	= { patturn-1, 0 }
							}
						});
						index	= groups.back().mg.index;
						break;
					}

					case ')' :
					{
						if (index == -1)
						{
							return	result_code::patturnerror;
						}
						group&	g			= groups[index];
						g.mg.match			= { g.mg.match.data()  , static_cast<size_t>(string - g.mg.match.data()) };
						g.mg.patturn		= { g.mg.patturn.data(), static_cast<size_t>(patturn - g.mg.patturn.data()) };
						index				= g.prev;
						break;
					}

					case '*' :
					{
						// '*' をスキップする
						for (;;)
						{
							// 最後まで '*' なら終了
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

						auto		groups_size		= groups.size();
						auto*		patturn_current	= patturn;
						auto		index_current	= index;
						while (string < string_end)
						{
							auto*	string_current	= string;
							auto	res				= match();
							if (res != result_code::nomatch)
							{
								return	res;
							}
							groups.resize(groups_size);
							string	= string_current + 1;
							patturn	= patturn_current;
							index	= index_current;
						}
						return	result_code::nomatch;
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
						while (string >= string_begin)
						{
							auto*	string_current	= string;
							auto	res				= match();
							if (res != result_code::nomatch)
							{
								return	res;
							}
							groups.resize(groups_size);
							string	= string_current-1;
							patturn	= patturn_current;
							index	= index_current;
						}
						return	result_code::nomatch;
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
							//patturn_error	= patturn;
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
								//patturn_error	= start;
								return	result_code::patturnerror;
							}

							if ((c = read_patturn()) == ']')
							{
								break;
							}
							if (c == 0)
							{
								//patturn_error	= patturn;
								return	result_code::patturnerror;
							}

							// エスケープコード
							if (c == '\\')
							{
								if (patturn == patturn_end)
								{
									//patturn_error	= patturn - 1;		// エスケープの後に文字がない
									return	result_code::patturnerror;
								}

								// 文字が一致すれば抜ける
								c = read_patturn();
								if (c == 0)
								{
									//patturn_error	= patturn;
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
									//patturn_error	= start;
									return	result_code::patturnerror;
								}
								++patturn;

								auto	current	= c;
								auto	last	= read_patturn();
								if (last == 0)
								{
									//patturn_error	= patturn;
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

					// エスケープシーケンス
					case '\\' :
					{
						if (patturn == patturn_end)
						{
							return	result_code::patturnerror;
						}
						c	= read_patturn();
						if (c == 0)
						{
							return	result_code::patturnerror;
						}
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

	};

	_		m;
	m.index			= -1;
	m.string		= _string.data();
	m.string_end	= _string.data() + _string.size();
	m.patturn		= _patturn.data();
	m.patturn_end	= _patturn.data() + _patturn.size();
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

	if constexpr (!std::is_same<Callback, std::nullptr_t>::value)
	{
		match_type	all	= {
			.index		= 0,
			.match		= _string,
			.patturn	= _patturn
		};
		_callback(all);
		for (auto& g : m.groups)
		{
			++g.mg.index;
			_callback(g.mg);
		}
	}
	return	(size_t)(1 + m.groups.size());
}

template<class T>
size_t wcmatch(T _string, T _patturn)
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	return	basic_match<std::nullptr_t, char_type>(view_type(_string), view_type(_patturn), true, nullptr);
}


template<class T>
size_t wcmatch(T _string, T _patturn, bool _cased)
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	return	basic_match<std::nullptr_t, char_type>(view_type(_string), view_type(_patturn), _cased, nullptr);
}


template<class T, class Callback>
size_t wcmatch(T _string, T _patturn, bool _cased, Callback&& _callback)
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	return	basic_match<Callback, char_type>(view_type(_string), view_type(_patturn), _cased, std::forward<Callback>(_callback));
}

template<class T, class Callback>
size_t wcmatch(T _string, T _patturn, Callback&& _callback)
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	return	basic_match<Callback, char_type>(view_type(_string), view_type(_patturn), true, std::forward<Callback>(_callback));
}


AMTRS_NAMESPACE_END
#endif
