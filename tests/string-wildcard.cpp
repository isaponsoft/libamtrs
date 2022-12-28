/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/tests.hpp>
#include <amtrs/string/wildcard.hpp>
#include <amtrs/system/log.hpp>

using namespace amtrs;

#define	_PRFX(_name)	string_wildcard_##_name


AMTRS_TEST_F(_PRFX(1_char))
{
	// strings.
	AMTRS_TEST_EQ(1, wcmatch("a", "a", true ));
	AMTRS_TEST_EQ(1, wcmatch("a", "a", false));
	AMTRS_TEST_EQ(0, wcmatch("a", "A", true ));
	AMTRS_TEST_EQ(1, wcmatch("a", "A", false));

	AMTRS_TEST_EQ(1, wcmatch("test", "test", true ));
	AMTRS_TEST_EQ(0, wcmatch("test", "TEST", true ));
	AMTRS_TEST_EQ(1, wcmatch("test", "TEST", false));
}

AMTRS_TEST_F(_PRFX(1_question))
{
	AMTRS_TEST_EQ(1, wcmatch("?", "a",  true ));
	AMTRS_TEST_EQ(1, wcmatch("?", "\\", true ));
	AMTRS_TEST_EQ(1, wcmatch("?", ".",  true ));
	AMTRS_TEST_EQ(1, wcmatch("?", "/",  true ));
	AMTRS_TEST_EQ(1, wcmatch("?", "[",  true ));
	AMTRS_TEST_EQ(1, wcmatch("?", "]",  true ));
	AMTRS_TEST_EQ(1, wcmatch("?", "A",  true));
	AMTRS_TEST_EQ(1, wcmatch("?", "Z",  true));
	AMTRS_TEST_EQ(1, wcmatch("?", "@",  true));
}

AMTRS_TEST_F(_PRFX(1_short))
{
	AMTRS_TEST_EQ(1, wcmatch("*Z",    "aZ",   true ));
	AMTRS_TEST_EQ(1, wcmatch("***Z",  "aZ",   true ));
	AMTRS_TEST_EQ(0, wcmatch("*Z",    "aZa",  true ));
	AMTRS_TEST_EQ(0, wcmatch("***Z",  "aZa",  true ));
	AMTRS_TEST_EQ(1, wcmatch("*Z*",   "aZa",  true ));
	AMTRS_TEST_EQ(1, wcmatch("***Z*", "aZa",  true ));
}


AMTRS_TEST_F(_PRFX(1_long))
{
	AMTRS_TEST_EQ(1, wcmatch("+Z",    "aZ",   true ));
	AMTRS_TEST_EQ(1, wcmatch("+++Z",  "aZ",   true ));
	AMTRS_TEST_EQ(0, wcmatch("+Z",    "aZa",  true ));
	AMTRS_TEST_EQ(0, wcmatch("+++Z",  "aZa",  true ));
	AMTRS_TEST_EQ(1, wcmatch("+Z+",   "aZa",  true ));
	AMTRS_TEST_EQ(1, wcmatch("+++Z+", "aZa",  true ));
}

AMTRS_TEST_F(_PRFX(2_charclass))
{
	AMTRS_TEST_EQ(1, wcmatch("[a-z]", "a", true ));
	AMTRS_TEST_EQ(1, wcmatch("[a-z]", "b", true ));
	AMTRS_TEST_EQ(1, wcmatch("[a-z]", "z", true ));
	AMTRS_TEST_EQ(0, wcmatch("[a-z]", "A", true ));
	AMTRS_TEST_EQ(0, wcmatch("[a-z]", "B", true ));
	AMTRS_TEST_EQ(0, wcmatch("[a-z]", "Z", true ));
	AMTRS_TEST_EQ(1, wcmatch("[a-z]", "A", false));
	AMTRS_TEST_EQ(1, wcmatch("[a-z]", "B", false));
	AMTRS_TEST_EQ(1, wcmatch("[a-z]", "Z", false));

	AMTRS_TEST_EQ(1, wcmatch("[a-z][a-z][a-z]", "abc", true ));
	AMTRS_TEST_EQ(1, wcmatch("[a-z][a-z][a-z]", "bxz", true ));
	AMTRS_TEST_EQ(1, wcmatch("[a-z][a-z][a-z]", "zax", true ));
	AMTRS_TEST_EQ(0, wcmatch("[a-z][a-z][a-z]", "ABC", true ));
	AMTRS_TEST_EQ(0, wcmatch("[a-z][a-z][a-z]", "BCD", true ));
	AMTRS_TEST_EQ(0, wcmatch("[a-z][a-z][a-z]", "ZXY", true ));
	AMTRS_TEST_EQ(1, wcmatch("[a-z][a-z][a-z]", "ABC", false));
	AMTRS_TEST_EQ(1, wcmatch("[a-z][a-z][a-z]", "BCD", false));
	AMTRS_TEST_EQ(1, wcmatch("[a-z][a-z][a-z]", "ZYX", false));
}


AMTRS_TEST_F(_PRFX(3_group))
{
	#define	RESET(_v)	for (auto& m: _v) { m = {}; }
	wildcard	m[10];

	RESET(m)
	wcmatch(m, "(test)", "test");
	AMTRS_TEST_EQ(m[0].match, "test");

	RESET(m)
	wcmatch(m, R"((abc)(def))", R"(abcdef)");
	AMTRS_TEST_EQ(m[0].match, R"(abc)");
	AMTRS_TEST_EQ(m[1].match, R"(def)");

	RESET(m)
	AMTRS_TEST_EQ(2, wcmatch(m, R"((?))", R"(a)", false));
	AMTRS_TEST_EQ(m[0].match, R"(a)");

	RESET(m)
	AMTRS_TEST_EQ(2, wcmatch(m, R"((*))", R"(abc)", false));
	AMTRS_TEST_EQ(m[0].match, R"(abc)");


	RESET(m)
	wcmatch(m, R"((abc)*)", R"(ABCDEF)", false);
	AMTRS_TEST_EQ(m[0].match, R"(ABC)");

	RESET(m)
	wcmatch(m, R"(*(def))", R"(ABCDEF)", false);
	AMTRS_TEST_EQ(m[0].match, R"(DEF)");

	RESET(m)
	wcmatch(m, R"((*)(def))", R"(ABCDEF)", false);
	AMTRS_TEST_EQ(m[0].match, R"(ABC)");
	AMTRS_TEST_EQ(m[1].match, R"(DEF)");

	RESET(m)
	wcmatch(m, R"(*(C:)*)", R"(C:Windows)", false);
	AMTRS_TEST_EQ(m[0].match, R"(C:)");

	RESET(m)
	wcmatch(m, R"(*(C*))", R"(CWindows)", false);
	AMTRS_TEST_EQ(m[0].match, R"(CWindows)");

	RESET(m)
	wcmatch(m, R"(*(C*))", R"(C:\Windows)", false);
	AMTRS_TEST_EQ(m[0].match, R"(C:\Windows)");

	RESET(m)
	wcmatch(m, R"((*C*))", R"(C:\Windows)", false);
	AMTRS_TEST_EQ(m[0].match, R"(C:\Windows)");

	RESET(m)
	wcmatch(m, R"(*(C*))", R"(C:\Windows)", false);
	AMTRS_TEST_EQ(m[0].match, R"(C:\Windows)");

	RESET(m)
	wcmatch(m, R"((*)C*)", R"(C:\Windows)", false);
	AMTRS_TEST_EQ(m[0].match, R"()");

	RESET(m)
	wcmatch(m, R"((*)(C*))", R"(C:\Windows)", false);
	AMTRS_TEST_EQ(m[0].match, R"()");
	AMTRS_TEST_EQ(m[1].match, R"(C:\Windows)");


	#undef	RESET
}
