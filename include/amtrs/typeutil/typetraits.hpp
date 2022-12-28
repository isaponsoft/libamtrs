/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__typeutil__typetraits__hpp
#define	__libamtrs__typeutil__typetraits__hpp
#include "eval_test.hpp"
AMTRS_NAMESPACE_BEGIN


/*
 * Check size() function
 */
struct has_size
{
	template<class T>
	static constexpr auto test(T& o) -> decltype(std::declval<size_t>() == o.size());
};
template<class T>	static constexpr bool	has_size_v			= eval_test<T, has_size>::value;
//template<class T>	static constexpr bool	enable_size_t		= std::enable_if_t< has_size_v<T> >;


/*
 * Check data() & size() function
 */
template<class It> struct has_data_size
{
	template<class T>
	static constexpr auto test(T& o) -> decltype(std::declval<It&>() == (o.data() + o.size()));
};
template<class R, class It>	static constexpr bool	has_has_data_size_v	= eval_test<R, has_data_size<It> >::value;
//template<class R, class It>	static constexpr bool	enable_data_suze_t	= std::enable_if_t<has_has_data_size_v<R, It> >;


/*
 * Check begin() & end() function
 */
template<class It> struct has_begin_end
{
	template<class T>
	static constexpr auto test(T& o) -> decltype((std::declval<It&>() == o.begin()) && (std::declval<It&>() == o.end()));
};
template<class R, class It>	static constexpr bool	has_begin_end_v		= eval_test<R, has_begin_end<It> >::value;
//template<class R, class It>	static constexpr bool	enable_begin_end_t	= std::enable_if_t<template has_begin_end_v<R, It> >;



template<class Dst> struct 	tets_can_copy_n
{
	template<class T> static constexpr auto test(T& o) -> decltype(std::copy_n(o.begin(), o.size(), std::declval<Dst&>()));
};
template<class T, class Dst>	static constexpr bool	can_copy_n	= eval_test<T, tets_can_copy_n<Dst>>::value;



AMTRS_NAMESPACE_END
#endif
