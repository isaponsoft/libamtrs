/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/system/log.hpp>
#include <amtrs/memory/stackbuff.hpp>
#include <amtrs/tests.hpp>
#define	_PRFX(_name)	memory_stackbuf_##_name
using namespace amtrs;



/*
 * stackbuf<> 破棄時に正しくデストラクタが呼ばれているかのテスト
 */
AMTRS_TEST_F(_PRFX(stackbuf))
{
	static int	count	= 0;

	struct	testst
	{
		testst(int v)
			: val(v)
		{
			++count;
		}

		~testst()
		{
			--count;
		}

		int	val;
	};

	{
		stackbuf<testst>	s;
		s.push(1);
		s.push(2);
		s.push(3);
		AMTRS_TEST_EQ(s.top().val, 3)
		s.pop();
		AMTRS_TEST_EQ(s.top().val, 2)
		s.push(4);
		AMTRS_TEST_EQ(s.top().val, 4)
	}
	AMTRS_TEST_EQ(count, 0);
}
