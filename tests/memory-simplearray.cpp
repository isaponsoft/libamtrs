/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <string>
#include <amtrs/tests.hpp>
#include <amtrs/memory/simplearray.hpp>
#include <amtrs/system/log.hpp>
using namespace amtrs;

#define	_PRFX(_name)	memory_simplearray_##_name


AMTRS_TEST_F(_PRFX(1))
{
	using	test_type	= simplearray<int>;
	int		x;

	test_type	a;
	AMTRS_TEST_EQ(a.size(), 0)

	// allocate
	test_type	b(100);
	AMTRS_TEST_EQ(b.size(), 100)
	x = 0;	for (auto& i : b) { i = x++; }
	x = 0;	for (auto& i : b) { AMTRS_TEST_EQ(i, x++); }

	// copy
	test_type	c(b);
	AMTRS_TEST_EQ(c.size(), 100)
	x = 0;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }

	// move
	test_type	d(std::move(b));
	AMTRS_TEST_EQ(d.size(), 100)
	AMTRS_TEST_EQ(b.size(), 0)
	AMTRS_TEST_TRUE(b.empty())
	x = 0;	for (auto& i : d) { AMTRS_TEST_EQ(i, x++); }

	// pop_front()
	c.pop_front();
	AMTRS_TEST_EQ(c.size(), 99)
	x = 1;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }
	c.pop_front();
	AMTRS_TEST_EQ(c.size(), 98)
	x = 2;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }

	// push_front();
	c.push_front(1);
	AMTRS_TEST_EQ(c.size(), 99)
	x = 1;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }
	c.push_front(0);
	AMTRS_TEST_EQ(c.size(), 100)
	x = 0;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }

	// push_back();
	c.push_back(100);
	AMTRS_TEST_EQ(c.size(), 101)
	x = 0;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }
	c.push_back(101);
	AMTRS_TEST_EQ(c.size(), 102)
	x = 0;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }

	// pop_back();
	c.pop_back();
	AMTRS_TEST_EQ(c.size(), 101)
	x = 0;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }
	c.pop_back();
	AMTRS_TEST_EQ(c.size(), 100)
	x = 0;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }

	// shift();
	auto	s1	= c.shift();
	AMTRS_TEST_EQ(c.size(), 99)
	AMTRS_TEST_EQ(s1, 0)
	x = 1;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }
	auto	s2	= c.shift();
	AMTRS_TEST_EQ(c.size(), 98)
	AMTRS_TEST_EQ(s2, 1)
	x = 2;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }

	c.push_front(1);
	c.push_front(0);
	AMTRS_TEST_EQ(c.size(), 100)
	x = 0;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }

	// resize
	c.resize(100);
	AMTRS_TEST_EQ(c.size(), 100)
	x = 0;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }

	c.resize(50);
	AMTRS_TEST_EQ(c.size(), 50)
	x = 0;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }

	c.resize(100);
	AMTRS_TEST_EQ(c.size(), 100)
	for (x = 0; x < 50; ++x) { AMTRS_TEST_EQ(c[x], x); }

	// assign (reset)
	c	= d;
	AMTRS_TEST_EQ(c.size(), 100)
	x = 0;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }


	b.resize(100);
	AMTRS_TEST_EQ(b.size(), 100)
	x = 100;	for (auto& i : b) { i = x++; }
	x = 100;	for (auto& i : b) { AMTRS_TEST_EQ(i, x++); }

	// push_front(it, it)
	c.push_front(b.begin(), b.end());	// [ 100, 101, 102, ...199, 0, 1, ...99 ]
	AMTRS_TEST_EQ(c.size(), 200)
	for (x = 100; x < 200; ++x) { AMTRS_TEST_EQ(c[x-100], x); }
	for (x =   0; x < 100; ++x) { AMTRS_TEST_EQ(c[x+100], x); }

	// assign (reset)
	c	= d;
	AMTRS_TEST_EQ(c.size(), 100)
	x = 0;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }

	// push_back(it, it)
	c.push_back(b.begin(), b.end());	// [ 0, 1, ...199 ]
	x = 0;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }

	// constractor(it, it)
	view<int>	v(c.begin() + 50, (size_t)100);
	c	= test_type(v.begin(), v.end());
	AMTRS_TEST_EQ(c.size(), 100)
	x = 50;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }

	// constractor(initializer_list{})
	c	= test_type({1, 2, 3, 4, 5});
	AMTRS_TEST_EQ(c.size(), 5)
	x = 1;	for (auto& i : c) { AMTRS_TEST_EQ(i, x++); }
}

static	int	sConstCount	= 0;
static	int	sMoveCount	= 0;
static	int	sDestCount	= 0;

struct	NoCopyTest
{
	NoCopyTest(int) { ++sConstCount; }
	NoCopyTest(NoCopyTest const&) = delete;
	NoCopyTest(NoCopyTest&&) = delete;
	~NoCopyTest() { ++sDestCount; }
};

AMTRS_TEST_F(_PRFX(2_constractor))
{
	using	test_type	= simplearray<NoCopyTest>;

	sConstCount	= 0;
	sDestCount	= 0;
	{
		test_type	a({0, 1, 2, 3, 4, 5});
		AMTRS_TEST_EQ(sConstCount, 6)
	}
	AMTRS_TEST_EQ(sConstCount, 6)
	AMTRS_TEST_EQ(sDestCount, 6)
}


struct	MoveTest
{
	MoveTest() { ++sConstCount; }
	MoveTest(int) { ++sConstCount; }
	MoveTest(MoveTest const&) = delete;
	MoveTest(MoveTest&&) { ++sMoveCount; }
	~MoveTest() { ++sDestCount; }
};

AMTRS_TEST_F(_PRFX(3_constractor))
{
	using	test_type	= simplearray<MoveTest>;

	sConstCount	= 0;
	sMoveCount	= 0;
	sDestCount	= 0;
	{
		test_type	a({0, 1, 2, 3, 4});
		AMTRS_TEST_EQ(sConstCount, 5)

		a.resize(10);
	}
	AMTRS_TEST_EQ(sConstCount, 10)		// a(5) + resize(10 - a.size())
	AMTRS_TEST_EQ(sMoveCount, 5)
	AMTRS_TEST_EQ(sDestCount, 15)
}
