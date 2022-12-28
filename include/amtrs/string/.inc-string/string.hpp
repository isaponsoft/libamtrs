/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__hpp
#define	__libamtrs__string__hpp
#include "amtrs.hpp"

#if	AMTRS_CPP_VER >= 14
#include "core/string/char_iterator.hpp"
#include "core/string/convert.hpp"
#include "core/string/split_iterator.hpp"
#endif


#if	AMTRS_CPP_VER >= 17
/* standard library */
#ifdef	__cplusplus
#include <cstring>
#include <cstdlib>
#include <iterator>
#include <regex>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#else
#include <string.h>
#endif

/* c utility */
#include ".inc-string/string-stringbuf.hpp"


/* c++ utility */
#ifdef	__cplusplus

#include ".inc-string/string-char8_t.hpp"
#include "sub/string/char_iterator.hpp"
#include ".inc-string/string-chartype.hpp"
#include ".inc-string/string-constring.hpp"
#include ".inc-string/string-convert.hpp"
#include ".inc-string/string-starts_with.hpp"
#include ".inc-string/string-find.hpp"
#include ".inc-string/string-find_view_if.hpp"
#include ".inc-string/string-join.hpp"
#include ".inc-string/string-lowercase.hpp"
#include ".inc-string/string-make_string_view.hpp"
#include ".inc-string/string-match_first_length.hpp"
#include ".inc-string/string-regex_match.hpp"
#include ".inc-string/string-regex_replace.hpp"
#include ".inc-string/string-readline.hpp"
#include ".inc-string/string-trim.hpp"
#include ".inc-string/string-textline.hpp"
#include ".inc-string/string-uppercase.hpp"
#include ".inc-string/string-utf8.hpp"
#include ".inc-string/get_quotetion_string.hpp"

AMTRS_NAMESPACE_BEGIN
std::string to_utf8(std::string_view _locale_string);
std::string to_current_locale(std::string_view _locale_string);
AMTRS_NAMESPACE_END
#endif
#endif

#endif
