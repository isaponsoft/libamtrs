/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/string.hpp>
#include <amtrs/tests.hpp>
using namespace amtrs;

#define	_PRFX(_name)	string_wildcard_##_name


AMTRS_TEST_F(_PRFX(1))
{
	// match test
	AMTRS_TEST_TRUE(wcmatch<std::string_view>("test", "[Tt]*t"));
}


AMTRS_TEST_F(_PRFX(2))
{
	// no match test
	AMTRS_TEST_FALSE(wcmatch<std::string_view>("test", "t*t?"));
}
