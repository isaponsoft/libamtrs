/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__type_utils__is_associative__hpp
#define __libamtrs__type_utils__is_associative__hpp
#include "def.hpp"
#include "void_type.hpp"
AMTRS_NAMESPACE_BEGIN


template<class T, class Key, class SFINES>
struct	is_associative
		: std::false_type
{
};

template<class T, class Key>
struct	is_associative<T, Key, typename void_type<decltype(std::declval<T>()[std::declval<Key>()])>::type>
		: std::true_type
{
};



AMTRS_NAMESPACE_END
#endif
