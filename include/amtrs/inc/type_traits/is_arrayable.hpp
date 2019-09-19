/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__type_utils__is_arrayable__hpp
#define __libamtrs__type_utils__is_arrayable__hpp
#include "def.hpp"
#include "is_associative.hpp"
AMTRS_NAMESPACE_BEGIN

template<class T, class SFINES>
struct	is_arrayable
		: std::false_type
{
};

template<class T>
struct	is_arrayable<T, typename std::enable_if<is_associative<T, int>::value>::type>
		: std::true_type
{};


AMTRS_NAMESPACE_END
#endif
