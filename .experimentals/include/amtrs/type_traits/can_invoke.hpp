/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__type_traits__can_invoke__hpp
#define __libamtrs__type_traits__can_invoke__hpp
#include <type_traits>
#include "def.hpp"
#include "function_type_of.hpp"
AMTRS_NAMESPACE_BEGIN

// ============================================================================
//!	@brief	指定する関数型が、Argsで指定されるパラメータで呼び出し可能か調べます。
// ----------------------------------------------------------------------------
//!	static_assert( amtrs::can_invoke<decltype(fopen), const char*, const char*>::value, "");
//!	static_assert(!amtrs::can_invoke<decltype(fclose), char>::value, "");
// ----------------------------------------------------------------------------
template<class F, class... Args>
struct	can_invoke;


namespace can_invoke_impl {


template<class F, class... Args>
struct	check
{
private:
	template<class X, class... A>
	static auto test(X& _f, A&&... _args) -> decltype(_f(_args...), std::true_type{});
	static auto test(...) -> std::false_type;
public:
	using	type	= decltype(test(std::declval<F&>(), std::declval<Args>()...));
};


} // namespace can_invoke_impl

template<class F, class... Args>
struct	can_invoke
		: public can_invoke_impl::check<F, Args...>::type
{
};





AMTRS_NAMESPACE_END
#endif
