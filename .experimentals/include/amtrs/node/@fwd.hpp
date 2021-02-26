/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__fwd__hpp
#define	__libamtrs__node__fwd__hpp
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN

//  forward declare.


template<class...>
struct	node_traits;


template<class T = float, class Traits = node_traits<T>, class Selector = void>
class	basic_node;


// ==================================================================
// flex_node
// ------------------------------------------------------------------
template<class SuperT, class...>
struct	flexbox_node_traits;


// ==================================================================
// itemable_node
// ------------------------------------------------------------------
template<class SuperT, class...>
struct	itemable_node_traits;


// ==================================================================
// paging_node
// ------------------------------------------------------------------
template<class SuperT, class...>
struct	paging_node_traits;


AMTRS_NAMESPACE_END
#endif
