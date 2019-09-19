/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__type_utils__has_type_list__hpp
#define __libamtrs__type_utils__has_type_list__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

// ============================================================================
//!	@brief	第１引数で指定する型が後続の型リストに含まれるか調べます。
// ----------------------------------------------------------------------------
//!	example)
//!		has_type_list<int  , char, int, double>::value	== std::true_type
//!		has_type_list<short, char, int, double>::value	== std::true_type
// ----------------------------------------------------------------------------
template <class Type, class... Args>
struct has_type_list;


template <class Type, class First, class... Args>
struct	has_type_list<Type, First, Args...>
		: std::conditional<std::is_same<Type, First>::value, std::true_type, has_type_list<Type, Args...>>::type
{};

template <class Type, class First>
struct	has_type_list<Type, First>
		: std::conditional<std::is_same<Type, First>::value, std::true_type, std::false_type>::type
{};



AMTRS_NAMESPACE_END
#endif
