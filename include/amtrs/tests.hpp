/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__tests__hpp
#define	__libamtrs__tests__hpp
#include "amtrs.hpp"
#include <iostream>

#define	AMTRS_TEST_NAMESPACE		AMTRS_NAMESPACE::test
#define	AMTRS_TEST_NAMESPACE_BEGIN	namespace AMTRS_TEST_NAMESPACE {
#define	AMTRS_TEST_NAMESPACE_END	}

AMTRS_TEST_NAMESPACE_BEGIN
extern	bool		failed;


int tests(int _argc, char** _args);
std::iostream& teststream();


template<class A, class B>
bool test_eq(int _line, A _a, B _b)
{
	if (!(_a == _b))
	{
		failed	= true;
		teststream() << "Line:" << _line << ":(" << _a << " != " << _b << ") ";
		return	false;
	}
	return	true;
}

template<class A, class B>
bool test_not_eq(int _line, A _a, B _b)
{
	if (!(_a != _b))
	{
		failed	= true;
		teststream() << "Line:" << _line << ":(" << _a << " == " << _b << ") ";
		return	false;
	}
	return	true;
}

inline bool test_true(int _line, bool _a)
{
	if (!(_a))
	{
		failed	= true;
		teststream() << "Line:" << _line << ":(not true) ";
		return	false;
	}
	return	true;
}


inline bool test_false(int _line, bool _a)
{
	if (_a)
	{
		failed	= true;
		teststream() << "Line:" << _line << ":(not false) ";
		return	false;
	}
	return	true;
}


struct	test_entry
{
	using	functype	= void (*)();

	char const*	name;
	functype	func;
	test_entry*	next;

	static inline test_entry*	first;

	test_entry(char const* n, functype f)
		: name(n)
		, func(f)
	{
		next	= first;
		first	= this;
	}
};
AMTRS_TEST_NAMESPACE_END

#define	__AMTRS_TEST_F_MAKENAME(_name, _num)	te_ ## _name ## _num
#define	_AMTRS_TEST_F_MAKENAME(_name, _num)		__AMTRS_TEST_F_MAKENAME(_name, _num)
#define	___AMTRS_TEST_F(_name)					static void _name(); static AMTRS_TEST_NAMESPACE::test_entry _AMTRS_TEST_F_MAKENAME(_name, __LINE__)(#_name, &_name); void _name()
#define	AMTRS_TEST_F(_name)						___AMTRS_TEST_F(_name)

#define	AMTRS_TEST_EQ(_a, _b)		{ if (!AMTRS_TEST_NAMESPACE::test_eq(__LINE__, _a, _b)) return; }
#define	AMTRS_TEST_NOTEQ(_a, _b)	{ if (!AMTRS_TEST_NAMESPACE::test_not_eq(__LINE__, _a, _b)) return; }
#define	AMTRS_TEST_TRUE(_a)			{ if (!AMTRS_TEST_NAMESPACE::test_true(__LINE__, _a)) return; }
#define	AMTRS_TEST_FALSE(_a)		{ if (!AMTRS_TEST_NAMESPACE::test_false(__LINE__, _a)) return; }

#endif
