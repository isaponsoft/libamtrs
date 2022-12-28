/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/system/log.hpp>

#include <amtrs/tests.hpp>
#include <amtrs/graphics/view2d.hpp>
#include <iostream>

using namespace amtrs;

#define	_PRFX(_name)	graphics_view2d_##_name


static int	img[17][16];



AMTRS_TEST_F(_PRFX(1_iterator))
{
	for (int y = 0; y < 17; ++y)
		for (int x = 0; x < 16; ++x)
			img[y][x] = y * 16 + x;

	view2d<int>		v({16, 17}, reinterpret_cast<int*>(img));

	// iterator
	{
		int	i	= 0;
		for (auto& c : v)
		{
			AMTRS_TEST_EQ((int)c, i)	
			++i;
		}
	}

	// iterator
	{
		auto	it	= v.end();
		AMTRS_TEST_EQ(v.size2().width,  16)	
		AMTRS_TEST_EQ(v.size2().height, 17)	
		int		i	= v.size2().width * v.size2().height - 1;
		for (;;)
		{
			--it;
			AMTRS_TEST_EQ((int)*it, i)	
			if (it == v.begin())
			{
				break;
			}
			--i;
		}
	}

	// reverse iterator
	{
		AMTRS_TEST_EQ(v.size2().width,  16)	
		AMTRS_TEST_EQ(v.size2().height, 17)	
		int		i	= v.size2().width * v.size2().height - 1;
		for (auto it = v.rbegin(); it != v.rend(); ++it)
		{
			AMTRS_TEST_EQ((int)*it, i)	
			--i;
		}
	}
}

AMTRS_TEST_F(_PRFX(2_subview))
{
	for (int y = 0; y < 17; ++y)
		for (int x = 0; x < 16; ++x)
			img[y][x] = y * 16 + x;

	view2d<int>		v({16, 17}, reinterpret_cast<int*>(img));

	{
		auto				v2(v.subbmp({4,4}));
		AMTRS_TEST_EQ(v2.size2().width , 12)	
		AMTRS_TEST_EQ(v2.size2().height, 13)	
		for (int y = 4; y < 16; ++y)
			for (int x = 4; x < 16; ++x)
			{
				AMTRS_TEST_EQ((v2[{x-4, y-4}]), y*16 + x)	
			}
	}

	{
		auto				v2(v.subbmp({1,1}, {2,2}));
		AMTRS_TEST_EQ(v2.size2().width , 2)
		AMTRS_TEST_EQ(v2.size2().height, 2)

		int		val[] = {1*16+1, 1*16+2, 2*16+1, 2*16+2, -1 };
		int		i	= 0;
		for (auto c : v2)
		{
			AMTRS_TEST_EQ(c, val[i++])
		}
	}

	{
		auto				v2(v.subbmp({15,15}, {10,10}));
		AMTRS_TEST_EQ(v2.size2().width , 1)
		AMTRS_TEST_EQ(v2.size2().height, 2)

		int		val[3*3];
		for (int y = 0; y < 2; ++y)
			for (int x = 0; x < 1; ++x)
				val[y*1+x] = (y+15) * 16 + (x+15);
		int		i	= 0;
		for (auto c : v2)
		{
			AMTRS_TEST_EQ(c, val[i++])
		}
	}

}
