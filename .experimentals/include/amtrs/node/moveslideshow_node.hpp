/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__moveslideshow_node__hpp
#define	__libamtrs__node__moveslideshow_node__hpp
#include "children_transform_node.hpp"
#include "updatable_node.hpp"
AMTRS_NAMESPACE_BEGIN


// ============================================================================
//! 子ノードが children_area() をはみ出す場合、水平方向にスライドショーを行う
// ----------------------------------------------------------------------------
//! 子ノードのX座標をスクロールさせます。
// ----------------------------------------------------------------------------
template<class SuperT>
class	horizonatlmoveslideshow_node
		: public updatable_node<children_transform_node<SuperT>>
{
public:
	static constexpr float	animation_start_wait	= 5.0f;

	using	super_type				= updatable_node<children_transform_node<SuperT>>;
	using	transform_component		= typename super_type::transform_component;
	using	super_type::super_type;

protected:
	void reset_moveslideshow_scroll()
	{
		super_type::set_children_position_x(0);
	}

protected:
	void on_update(float _delta) override
	{
		super_type::on_update(_delta);
		if (mAutoScrollWidth != 0)
		{
			if (mStopWait > 0)
			{
				mStopWait	-= _delta;
				if (mStopWait > 0)
				{
					return;
				}
				mStopWait	= 0;
			}
			auto	cx	= super_type::children_position().x;
			if (cx > 0)
			{
				cx -= 4.0f;
				if (cx <= 0)
				{
					cx			= 0.0f;
					mStopWait	= animation_start_wait;
				}
			}
			else
			{
				cx -= 2.0f;
			}
			if (cx < mAutoScrollWidth)
			{
				cx	= super_type::children_area().width;
			}
			super_type::set_children_position_x(cx);
		}
	}

	void on_enter() override
	{
		super_type::on_enter();
		status_check();
	}

	void on_child(typename super_type::node_type* _child, typename super_type::on_child_event _event) override
	{
		super_type::on_child(_child, _event);
		status_check();
	}

	void on_children_transform_change(transform_component* _child) override
	{
		super_type::on_children_transform_change(_child);
		status_check();
	}

	void on_children_size_change(transform_component* _child) override
	{
		super_type::on_children_size_change(_child);
		status_check();
	}

private:
	void status_check()
	{
		if (!mChecking)
		{
			mChecking	= true;
			this->schedule_once([this](float)
			{
				auto	cb	= super_type::children_bounds();
				auto	cs	= super_type::children_area();
				mAutoScrollWidth	= 0;//std::max<float>(0, cb.width - cs.width);
				if (cb.width > cs.width)
				{
					mAutoScrollWidth	= -cb.width;
				}
				else
				{
					super_type::set_children_position_x(0);
				}
				mChecking	= false;
			});
		}
	}

	bool	mChecking			= false;
	float	mAutoScrollWidth	= 0;
	float	mStopWait			= animation_start_wait;
};


AMTRS_NAMESPACE_END
#endif
