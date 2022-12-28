/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__multibyte__hpp
#define	__libamtrs__string__multibyte__hpp
#include "text.hpp"
AMTRS_STRING_NAMESPACE_BEGIN

/*
 * 先頭からマルチバイトを考慮して有効な文字列の長さを返す。
 * NUL文字またはマルチバイトの無効な文字列があった場合そこまでの長さを返す。
 */
AMTRS_API text::size_type mbstrlen(text _text);

/*
 * 先頭からマルチバイトを考慮して有効な文字列を切り抜いて返す。
 */
AMTRS_API text mbtext(text _text);


AMTRS_STRING_NAMESPACE_END
#endif
