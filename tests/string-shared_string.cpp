/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/tests.hpp>
#include <amtrs/string/shared_string.hpp>
#include <amtrs/system/log.hpp>

using namespace amtrs;

#define	_PRFX(_name)	shared_string_##_name

using	string_type	= shared_string;

AMTRS_TEST_F(_PRFX(1))
{
	string_type	s1("Hello world");
	string_type	s2({ 'H', 'e', 'l', 'l', 'o'});
	text		v1(s1);
	string_type	s3(v1.begin(), v1.end());

	AMTRS_TEST_EQ(s1.size(), 11)
	AMTRS_TEST_EQ((text)s1, "Hello world")
	AMTRS_TEST_EQ((text)s2, "Hello")
	AMTRS_TEST_EQ((text)s3, "Hello world")
	AMTRS_TEST_TRUE(s1 == s3)
	AMTRS_TEST_FALSE(s1 == s2)
	AMTRS_TEST_TRUE(s1 != s2)
	AMTRS_TEST_TRUE(s1 == "Hello world")
	AMTRS_TEST_TRUE(s2 == "Hello")
	AMTRS_TEST_TRUE(s3 == "Hello world")
	AMTRS_TEST_FALSE(s1 == "Hello world.")
	AMTRS_TEST_FALSE(s2 == "Hello.")
	AMTRS_TEST_FALSE(s3 == "Hello world.")
}
