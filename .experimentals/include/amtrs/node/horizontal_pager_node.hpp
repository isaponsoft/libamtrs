/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__horizontal_pager_node__hpp
#define	__libamtrs__node__horizontal_pager_node__hpp
#include "flickable_pager_node.hpp"
AMTRS_NAMESPACE_BEGIN

template<class NodeT>
class	horizontal_pager_node
		: public flickable_pager_node<NodeT>
{
public:
	using	super_type	= flickable_pager_node<NodeT>;

	horizontal_pager_node()
		: super_type(uiutils::scroll::axis_x)
	{}

protected:
	virtual vec3<float> compute_page_position(typename super_type::index_type _page) const override
	{
		return	{ this->size().width * float(_page), 0, 0 };
	}
};


AMTRS_NAMESPACE_END
#endif
