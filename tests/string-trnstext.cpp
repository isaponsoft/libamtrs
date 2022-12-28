/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/tests.hpp>
#include <amtrs/string/trnstext.hpp>
#include <amtrs/system/log.hpp>

using namespace amtrs;

#define	_PRFX(_name)	string_trnstext_##_name
#define	_TEST_EQ(_key, _val)	{ auto result = _TT(_key); AMTRS_TEST_EQ((text)result, (text)_val) }

AMTRS_TEST_F(_PRFX(1_set))
{
	trnstext::set(R"(
		@"Hello world {} {}"
		+"こんにちは、世界 {} {}"

		@"This is a pen."
		+"tHIS IS A PEN,"
	)");
	_TEST_EQ("Hello world {} {}", "こんにちは、世界 {} {}")
	_TEST_EQ("This is a pen.", "tHIS IS A PEN,")
	_TEST_EQ("No treanslate data.", "No treanslate data.")
}
