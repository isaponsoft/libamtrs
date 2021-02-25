/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__node_trats__hpp
#define	__libamtrs__node__node_trats__hpp
AMTRS_NAMESPACE_BEGIN


template<class T>
struct	node_traits<T>
{
	using	node_type	= basic_node<T, node_traits<T>>;
};


AMTRS_NAMESPACE_END
#endif
