/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__scrollbox_node__hpp
#define	__libamtrs__node__scrollbox_node__hpp
#include "scroll_node.hpp"
AMTRS_NAMESPACE_BEGIN


// ****************************************************************************
//! 子ノードの位置とサイズに合わせて自動的にスクロール範囲を調整します。
// ----------------------------------------------------------------------------
template<class SuperT>
class	scrollbox_node
		: public scroll_node<SuperT>
{
public:
	using	super_type				= scroll_node<SuperT>;
	using	transform_component		= typename super_type::transform_component;

	void set_scroll_margin(float _l, float _r, float _t, float _b)
	{
		mLeft	= _l;
		mRight	= _r;
		mTop	= _t;
		mBottom	= _b;
		update_scroll_range();
	}


protected:
	virtual void on_children_transform_change(transform_component* _child) override
	{
		super_type::on_children_transform_change(_child);
		update_scroll_range();
	}

	void update_scroll_range()
	{
		auto	thisSize	= this->size();
		auto	childArea	= super_type::children_bounds();
		super_type::set_scroll_range_x({std::min<float>(-mRight,   thisSize.width  - childArea.width   - mRight ),  mLeft});
		super_type::set_scroll_range_y({std::min<float>(-mBottom,  thisSize.height - childArea.height  - mBottom),  mTop });
	}

	float	mLeft	= 0;
	float	mRight	= 0;
	float	mTop	= 0;
	float	mBottom	= 0;
};


AMTRS_NAMESPACE_END
#endif
