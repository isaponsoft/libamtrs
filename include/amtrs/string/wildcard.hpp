/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__wildcard_hpp
#define	__libamtrs__string__wildcard_hpp
#include "../amtrs.hpp"
#include "text.hpp"
AMTRS_NAMESPACE_BEGIN

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

template<class CharT>
struct	basic_wildcard
{
	using	view_type	= basic_text<CharT>;
	using	group_index	= int;

	group_index		index;	
	view_type		match;
	view_type		patturn;
};

using	wildcard	= basic_wildcard<char>;
using	wwildcard	= basic_wildcard<wchar_t>;



AMTRS_API size_t wcmatch   ( wildcard* _results, text  _patturn, text  _string, bool _cased = true);
AMTRS_API size_t wcmatch   (wwildcard* _results, wtext _patturn, wtext _string, bool _cased = true);
AMTRS_API size_t wcmatch_mb( wildcard* _results, text  _patturn, text  _string, bool _cased = true);


/*
 * Ascii (Not multibytes)
 */

AMTRS_API size_t wcmatch(text  _patturn, text  _string);
AMTRS_API size_t wcmatch(wtext _patturn, wtext _string);
AMTRS_API size_t wcmatch(text  _patturn, text  _string, bool _cased);
AMTRS_API size_t wcmatch(wtext _patturn, wtext _string, bool _cased);
AMTRS_API size_t wcmatch(text  _patturn, text  _string, bool _cased, bool(*_callback)(void*,wildcard &), void* _userval);
AMTRS_API size_t wcmatch(wtext _patturn, wtext _string, bool _cased, bool(*_callback)(void*,wwildcard&), void* _userval);

template<class Callback>
size_t wcmatch(text _patturn, text _string, bool _cased, Callback&& _callback)
{
	return	wcmatch(_patturn, _string, _cased, [](void* _p, wildcard& _w) -> bool
			{
				if constexpr (std::is_same<void, decltype(std::declval<Callback&>()(std::declval<wildcard&>()))>::value)
				{
					(*reinterpret_cast<decltype(&_callback)>(_p))(_w);
					return	true;
				}
				else
				{
					return	(*reinterpret_cast<decltype(&_callback)>(_p))(_w);
				}
			}, &_callback);
}

template<class Callback>
size_t wcmatch(text _patturn, text _string, Callback&& _callback)
{
	return	wcmatch(_patturn, _string, true, std::forward<Callback>(_callback));
}

template<class Callback>
size_t wcmatch(wtext _patturn, wtext _string, bool _cased, Callback&& _callback)
{
	return	wcmatch(_patturn, _string, _cased, [](void* _p, wwildcard& _w) -> bool
			{
				if constexpr (std::is_same<void, decltype(std::declval<Callback&>()(std::declval<wwildcard&>()))>::value)
				{
					(*reinterpret_cast<decltype(&_callback)>(_p))(_w);
					return	true;
				}
				else
				{
					return	(*reinterpret_cast<decltype(&_callback)>(_p))(_w);
				}
			}, &_callback);
}

template<class Callback>
size_t wcmatch(wtext _patturn, wtext _string, Callback&& _callback)
{
	return	wcmatch(_patturn, _string, true, std::forward<Callback>(_callback));
}

/*
 * Multibytes
 */
AMTRS_API size_t wcmatch_mb(text _patturn, text _string);
AMTRS_API size_t wcmatch_mb(text _patturn, text _string, bool _cased);
AMTRS_API size_t wcmatch_mb(text _patturn, text _string, bool _cased, bool(*_callback)(void*,wildcard&), void* _userval);

template<class Callback>
size_t wcmatch_mb(text _patturn, text _string, bool _cased, Callback&& _callback)
{
	return	wcmatch_mb(_patturn, _string, _cased, [](void* _p, wildcard& _w) -> bool
			{
				if constexpr (std::is_same<void, decltype(std::declval<Callback&>()(std::declval<wildcard&>()))>::value)
				{
					(*reinterpret_cast<decltype(&_callback)>(_p))(_w);
					return	true;
				}
				else
				{
					return	(*reinterpret_cast<decltype(&_callback)>(_p))(_w);
				}
			}, &_callback);
}

template<class Callback>
size_t wcmatch_mb(text _patturn, text _string, Callback&& _callback)
{
	return	wcmatch_mb(_patturn, _string, true, std::forward<Callback>(_callback));
}

AMTRS_NAMESPACE_END
#endif
