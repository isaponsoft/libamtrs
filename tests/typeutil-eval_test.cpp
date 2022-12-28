/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/tests.hpp>
#include <amtrs/typeutil/eval_test.hpp>
using namespace amtrs;

#define	_PRFX(_name)	typeutil_eval_test_##_name


struct	TestCase
{
	TestCase() {}
	TestCase(int) {}
	void f1() {}
	void f2(int) {}

	int		v1;
	int*	v2;
};


struct	HasConstractor
{
	template<class T>
	auto test(T& o) -> decltype(new T());
};

struct	HasConstractor1
{
	template<class T>
	auto test(T& o) -> decltype(new T(1));
};

struct	HasF1
{
	template<class T>
	auto test(T& o) -> decltype(o.f1());
};

struct	HasF2
{
	template<class T>
	auto test(T& o) -> decltype(o.f2(1));
};

struct	HasF2_ERR
{
	template<class T>
	auto test(T& o) -> decltype(o.f2());
};

struct	HasV1
{
	template<class T>
	auto test(T& o) -> decltype(o.v1);
};

struct	HasV2
{
	template<class T>
	auto test(T& o) -> decltype(o.v2);
};

struct	HasV2_ERR
{
	template<class T>
	auto test(T& o) -> decltype(o.v2 = 1);
};


AMTRS_TEST_F(typeutil_eval_test)
{
	AMTRS_TEST_EQ((eval_test<TestCase, HasConstractor>::value), true)
	AMTRS_TEST_EQ((eval_test<TestCase, HasConstractor1>::value), true)
	AMTRS_TEST_EQ((eval_test<TestCase, HasF1>::value), true)
	AMTRS_TEST_EQ((eval_test<TestCase, HasF2>::value), true)
	AMTRS_TEST_EQ((eval_test<TestCase, HasF2_ERR>::value), false)
	AMTRS_TEST_EQ((eval_test<TestCase, HasV1>::value), true)
	AMTRS_TEST_EQ((eval_test<TestCase, HasV2>::value), true)
	AMTRS_TEST_EQ((eval_test<TestCase, HasV2_ERR>::value), false)
}
