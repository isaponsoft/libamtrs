/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__type_utils__type_list__hpp
#define __libamtrs__type_utils__type_list__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(type_list)

template<std::size_t I, class Find, class... Types>
struct	index_of;

template<std::size_t I, class Find, class First, class... Types>
struct	index_of<I, Find, First, Types...>
		: std::conditional<std::is_same<Find, First>::value
			, std::integral_constant<std::size_t, I>
			, index_of<I+1, Find, Types...>
		>::type
{};


template<std::size_t I, class Find, class Last>
struct	index_of<I, Find, Last>
		:
std::conditional<std::is_same<Find, Last>::value
			, std::integral_constant<std::size_t, I>
			, void
		>::type
{};

AMTRS_IMPLEMENTS_END(type_list)


// ============================================================================
//!	@brief	可変テンプレート引数から、指定位置の型を取り出す。
//!			std::tuple<> と std::tuple_element<> を扱いやすくしたもの。
// ----------------------------------------------------------------------------
//!	using	ltype	= type_list<int, float, char*>;
//!	using	tuple_t	= ltype::tuple_type;
//!	using	second	= ltype::element_type<1>;
//!	static_assert(std::is_same<tuple_t, std::tuple<int, float, char*>>::value, "no");
//!	static_assert(std::is_same<second, float>::value, "no");
// ----------------------------------------------------------------------------
template <class... Args>
struct type_list
{
	using				tuple_type		= std::tuple<Args...>;

	template<std::size_t N>
	using				element_type	= typename std::tuple_element<N, tuple_type>::type;

	template<class T>
	using				index_type		= AMTRS_IMPLEMENTS(type_list)::index_of<0, T, Args...>;
};


// ============================================================================
//!	@brief	type_list<...>::element_type<N> に対するエイリアス。
// ----------------------------------------------------------------------------
template<std::size_t N, class... Args>
using	type_list_type		= typename type_list<Args...>::template element_type<N>;


AMTRS_NAMESPACE_END
#endif
