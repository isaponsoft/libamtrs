/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__text__hpp
#define	__libamtrs__string__text__hpp
#include "../amtrs.hpp"
#include "../memory/view.hpp"
#include <algorithm>
#include <iosfwd>
#include <string_view>
AMTRS_NAMESPACE_BEGIN
static char constexpr utf8bom[3] = { (char)0xEF, (char)0xBB, (char)0xBF };


template<class C>
using	basic_text		= view<C const>;

using	text			= basic_text<char>;
using	wtext			= basic_text<wchar_t>;


AMTRS_NAMESPACE_END
AMTRS_STRING_NAMESPACE_BEGIN

inline amtrs_constexpr bool starts_with(text _s, text::value_type const* _t) { return _s.starts_with(_t); }
inline amtrs_constexpr bool starts_with(text _s, text const& _t) { return _s.starts_with(_t); }
inline amtrs_constexpr bool starts_with(wtext _s, wtext::value_type const* _t) { return _s.starts_with(_t); }
inline amtrs_constexpr bool starts_with(wtext _s, wtext const& _t) { return _s.starts_with(_t); }

/*
 * アルファベットの大文字を小文字に変換します
 */
AMTRS_API void lowercase(char* _begin, char* _end);


/*
 * アルファベットの小文字を大文字に変換します
 */
AMTRS_API void uppercase(char* _begin, char* _end);


/*
 * 先頭の空白や改行を取り除いた領域を返します。
 */
AMTRS_API text skip_blank(text _code);
AMTRS_API char const* skip_blank(char const* _begin, char const* _end);


/*
 * \n,\r,\r\n,\n\r を一つだけ読み飛ばします。
 */
AMTRS_API text skip_one_return(text _code);
AMTRS_API char const* skip_one_return(char const* _begin, char const* _end);

/*
 * _code の先頭と最後尾から調べ、空白や改行を取り除いた領域を返します。
 */
AMTRS_API text trim_blank(text _code);

/*
 * _code の先頭と最後尾から調べ、空白や改行、括弧(),[],{}を取り除いた領域を返します。
 */
AMTRS_API text trim_brackets(text _code);

/*
 * _code の先頭と最後尾から調べ、空白や改行、括弧()を取り除いた領域を返します。
 */
AMTRS_API text trim_circle_brackets(text _code);

/*
 * UTF-8のBOMがあればスキップする
 */
AMTRS_API text skip_utf8_bom(text _code);


AMTRS_STRING_NAMESPACE_END
#endif
