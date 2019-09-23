/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
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
	template<class... U>
	static auto c(U&&... _a) -> decltype(std::declval<F>()(std::forward<U>(_a)...), std::true_type{});
	static auto c(...)  -> decltype(std::false_type{});
	using	type	= decltype(c(std::declval<Args>()...));
};


} // namespace can_invoke_impl

template<class F, class... Args>
struct	can_invoke
		: public can_invoke_impl::check<typename function_type_of<F>::type, Args...>::type
{
};

AMTRS_NAMESPACE_END
#endif
