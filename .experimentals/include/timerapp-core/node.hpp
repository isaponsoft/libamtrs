/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__timerapp_code__node__hpp
#define	__timerapp_code__node__hpp
#include <amtrs/graphics/@fwd.hpp>
#include <amtrs/node/node.hpp>
AMTRS_NAMESPACE_BEGIN
#if	0
template<class SuperT>
struct	flexbox_node_traits<SuperT>
		: flexbox_node_traits<SuperT, void>
{
	template<class N, class This>
	void add_item(N* _node, This* _this)
	{
		_node->set_scale({1, 0, 0});
		_node->run_action(new scale_by_action<>({1, 1, 1}, 0.15f));
	}

	template<class N, class This>
	void set_item_position(N* _node, vec3<float> const& _pos, This* _this)
	{
		_node->set_position(_pos);
	}
};


template<class SuperT>
struct	paging_node_traits<SuperT>
{
	template<class T, class O, class Callback>
	void page_change_transform(T* _transform, vec3<float> _pos, O* _owner, Callback&& _callback)
	{
		_transform->set_position(_pos);
		_callback();
	}
};
#endif
AMTRS_NAMESPACE_END
#endif
