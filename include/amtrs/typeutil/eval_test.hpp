/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__typeutil__eval_test__hpp
#define	__libamtrs__typeutil__eval_test__hpp
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(eval_test)
template<class T, class Test>
class	eval_test
{
	template<class U> static constexpr auto test(U& o) -> decltype(Test{}.template test<U>(o), std::true_type{});
	                  static constexpr auto test(... ) -> std::false_type;
public:
	using	type	= decltype(test(std::declval<T&>()));
};
AMTRS_IMPLEMENTS_END(eval_test)


// Test::test(T&) を呼び出し、呼び出し可能なら true, 不可能の場合は false になります。
//
//
template<class T, class Test>
using	eval_test	= typename AMTRS_IMPLEMENTS(eval_test)::template eval_test<T, Test>::type;


/*
	struct	MyClass
	{
		void func();
	};


	struct	has_func
	{
		template<class T>
		auto test(T& o) -> decltype(o.func());
	};

	struct	has_value
	{
		template<class T>
		auto test(T& o) -> decltype(o.val);
	};


	eval_test<MyClass, has_func>::value		// true
	eval_test<MyClass, has_value>::value	// false

*/


AMTRS_NAMESPACE_END
#endif
