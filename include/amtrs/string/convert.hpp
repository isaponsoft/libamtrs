/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__convert__hpp
#define	__libamtrs__string__convert__hpp
#include "sstring.hpp"
AMTRS_NAMESPACE_BEGIN

// wide char => UTF-8
AMTRS_API sstring wc_to_c8(wtext _text);

// UTF-8 => wide char
AMTRS_API wsstring c8_to_wc(text _text);

// locale => utf8
AMTRS_API sstring cr_to_c8(text _text);

// utf8 => locale
AMTRS_API sstring c8_to_cr(text _text);

AMTRS_NAMESPACE_END
#endif
