/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__trnstext__hpp
#define	__libamtrs__string__trnstext__hpp
#include "sstring.hpp"
#include "text.hpp"
AMTRS_NAMESPACE_BEGIN namespace trnstext {

#ifndef	_TT
#define	_TT(_txt)	amtrs::trnstext::get(_txt)
#endif



bool add(text _transdata);
text get(text _txt);
bool set(text _transdata);
bool set_from_file(text _path, text _filename);

bool add(text _group, text _transdata);
text get(text _group, text _txt);
bool set(text _group, text _transdata);
bool set_from_file(text _group, text _path, text _filename);



} AMTRS_NAMESPACE_END
#endif
