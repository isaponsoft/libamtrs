/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__type_traits__function_type_of__hpp
#define __libamtrs__type_traits__function_type_of__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


// ============================================================================
//!	@brief	指定する型の関数型を取得します。
// ----------------------------------------------------------------------------
//!	using	type1	= function_type_of<decltype(fopen)>::type;
//!	using	type2	= FILE*(const char*, const char*);
//!	std::is_same<type1, type2>::value == 1
// ----------------------------------------------------------------------------
template<class... F>
struct	function_type_of;



namespace function_type_of_impl {


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
	static auto c(U...) -> decltype(&T::operator());
	static auto c(...)  -> decltype(std::false_type{});

	using	type	= decltype(c(std::declval<T>()));
};

// 関数オブジェクトからクラスの情報を削除する
template<class... F>
struct	remove_class_scope;

template<class F>
struct	remove_class_scope<F>
{
	using	type	= F;
};

template<class R, class... Args>
struct	remove_class_scope<R(Args...)>
{
	using	type	= R(Args...);
};

template<class R, class C, class... Args>
struct	remove_class_scope<R(C::*)(Args...)>
{
	using	type	= R(Args...);
};

template<class R, class C, class... Args>
struct	remove_class_scope<R(C::*)(Args...) const>
{
	using	type	= R(Args...);
};


} // namespace function_type_of_impl

template<class F>
struct	function_type_of<F>
{
	using	type	= typename function_type_of_impl::remove_class_scope<typename function_type_of_impl::check<F>::type>::type;
};

// メソッド
template<class R, class C, class... Args>
struct	function_type_of<R(C::*)(Args...)>
{
	using	type	= R(C::*)(Args...);
};

// メソッド
template<class R, class C, class... Args>
struct	function_type_of<R(C::*)(Args...) const>
{
	using	type	= R(C::*)(Args...);
};

// 関数ポインタ
template<class R, class... Args>
struct	function_type_of<R(*)(Args...)>
{
	using	type	= R(Args...);
};

// 関数型
template<class R, class... Args>
struct	function_type_of<R(Args...)>
{
	using	type	= R(Args...);
};


template<>
struct	function_type_of<std::false_type>
{};


template<class...>
struct	callable_result_of;

template<class F>
struct	callable_result_of<F>
{
	using	type	= typename callable_result_of<typename function_type_of<F>::type>::type;
};

template<class R, class C, class... Args>
struct	callable_result_of<R(C::*)(Args...)>
{
	using	type	= R;
};

template<class R, class C, class... Args>
struct	callable_result_of<R(C::*)(Args...) const>
{
	using	type	= R;
};

template<class R, class... Args>
struct	callable_result_of<R(*)(Args...)>
{
	using	type	= R;
};

template<class R, class... Args>
struct	callable_result_of<R(Args...)>
{
	using	type	= R;
};


AMTRS_NAMESPACE_END
#endif
