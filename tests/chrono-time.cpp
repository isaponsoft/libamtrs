/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/chrono/chrono.hpp>
#include <amtrs/tests.hpp>
#define	_PRFX(_name)	chrono_time_##_name
using namespace amtrs;
using namespace amtrs::chrono;

AMTRS_TEST_F(_PRFX(days))
{
	chrono::days	d(2022, 3, 4);
	chrono::times	t(1, 9, 56);
	chrono::timev	v(d, t);

	AMTRS_TEST_EQ(v.format("%Y/%m/%d %H:%M:%S"), "2022/03/04 01:09:56")

	v	= days(2021, 1, 2);
	AMTRS_TEST_EQ(v.format("%Y/%m/%d %H:%M:%S"), "2021/01/02 01:09:56")

	v	= times(3, 16, 22);
	AMTRS_TEST_EQ(v.format("%Y/%m/%d %H:%M:%S"), "2021/01/02 03:16:22")
}
