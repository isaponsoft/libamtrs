/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__type_traits__is_callable__hpp
#define	__libamtrs__type_traits__is_callable__hpp
#include "def.hpp"
#if 0
namespace std
{
template<class... F>
struct	is_callable;


namespace is_callable_impl {


template<class T, class SFINES = void>
struct	check
{
	using	type	= std::false_type;
};

// 関数オブジェクト（クロージャ）
template<class T>
struct	check<T, typename std::enable_if<std::is_class<T>::value>::type>
{
	template<class... U>
	static auto c(U...) -> decltype(&T::operator(), std::true_type{});
	static auto c(...)  -> decltype(std::false_type{});

	using	type	= decltype(c(std::declval<T>()));
};

} // namespace is_callable_impl

template<class... F>
struct	is_callable
		: is_callable_impl::check<F...>::type
{};

// メソッド
template<class R, class C, class... Args>
struct	is_callable<R(C::*)(Args...)>
		: std::true_type
{};

// 関数
template<class R, class... Args>
struct	is_callable<R(*)(Args...)>
		: std::true_type
{};

// 関数型
template<class R, class... Args>
struct	is_callable<R(Args...)>
		: std::true_type
{};

	
} // namespace std
#endif
#endif

