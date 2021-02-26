/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
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
