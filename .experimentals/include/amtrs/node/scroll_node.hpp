/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__scroll_node__hpp
#define	__libamtrs__node__scroll_node__hpp
#include "node.hpp"
#include "ui_utils.hpp"
#include "updatable_node.hpp"
AMTRS_NAMESPACE_BEGIN
using namespace uiutils;


// ============================================================================
//! ノードに対してスクロール機能を持たせます。
// ----------------------------------------------------------------------------
//! 
// ----------------------------------------------------------------------------
template<class NodeT>
class	scroll_node
		: public updatable_node<NodeT>
{
	using	super_type		= updatable_node<NodeT>;
public:

	using	matrix_type		= typename NodeT::matrix_type;
	using	position_type	= typename NodeT::position_type;

	static constexpr auto	axis_x	= scroll::axis_x;
	static constexpr auto	axis_y	= scroll::axis_y;

	using	super_type::super_type;

	scroll_node(uint32_t _scroll_axis = scroll::axis_y)
		: mScroller(_scroll_axis)
	{}


	void set_multi_scroll(bool _enable)
	{
		mScroller.set_multi_scroll(_enable);
	}


	vec3<float> get_pinch_scale() const noexcept
	{
		return	mScrollMatrix.scale();
	}

	void set_scroll_enable(bool _enable) noexcept
	{
		mEnableScroll	= _enable;
	}

	virtual void on_input_cancel() override
	{
		super_type::on_input_cancel();
		mScroller.cancel(*this);
	}


	virtual typename NodeT::node_type* on_input(typename super_type::pointer_event* _e) override
	{
		auto	mat		= super_type::world_matrix().inverse();
		auto	mouse	= _e->event()->get_mouse_event();
		if (mouse && mEnableScroll)
		{
			if (mScroller.is_capturing())
			{
				auto	pos	= vec2<float>(vec3<float>(mouse->template get_point<float>(),0) * mat);
				if (mouse->type == input_event::mouse)
				{
					mScroller.input(*this, mouse, &pos);

					auto	of	= get_overfllow();
					if ((of.x != 0 || of.y != 0) && ((mOverfllowPrev != of) || (mouse->state == input_event::release)))
					{
						vec2<float>	scroll(mScroller.position());
						mOverfllowPrev	= of;
						auto	of2(of);
						on_overfllow(of2, mouse->state == input_event::release);
						scroll	+= of2 - of;
						mScroller.set_position(scroll);
					}
				}
				return	this;
			}


			if (auto r = super_type::on_input(_e); r)
			{
				mScroller.release();
				return	r;
			}

			auto	pos	= vec2<float>(vec3<float>(mouse->template get_point<float>(),0) * mat);
			if (mouse->type == input_event::mouse)
			{
				mScroller.input(*this, mouse, &pos);
			}

			return	mScroller.is_capturing() ? this : nullptr;
		}
		else
		{
			return	super_type::on_input(_e);
		}
	}


	virtual void on_update(float _delta) override
	{
		super_type::on_update(_delta);
		mScroller.update(_delta);

		auto	now	= vec3<float>(mScroller.position());
		if (mScrollPrev != now)
		{
			vec2<float>	scroll(now.x, now.y);
			on_scrolling(scroll);
			mScroller.set_position(scroll);

			auto	of	= get_overfllow();
			if ((of.x != 0 || of.y != 0) && (mOverfllowPrev != of))
			{
				mOverfllowPrev	= of;
				auto	of2(of);
				on_overfllow(of2, false);
				scroll	+= of2 - of;
				mScroller.set_position(scroll);
			}

			now	= vec3<float>(mScroller.position());
			if ((scroll.x != mScrollPrev.x) || (scroll.y != mScrollPrev.y))
			{
				mScrollMatrix *= mat44<float>::translate(now - mScrollPrev);
				mScrollPrev = now;
				this->on_coordinate_modify();
			}
		}
	}

	//! 範囲外にはみ出している量を取得します。
	vec2<float> get_overfllow() const noexcept { return mScroller.get_overfllow(); }

	vec2<float> get_scroll() const noexcept { return vec2<float>(mScroller.position()); }

	void set_scroll(vec2<float> _scroll)
	{
		if (_scroll != mScroller.position())
		{
			mScroller.set_position(_scroll);
			on_scrolling(_scroll);
			mOverfllowPrev	= get_overfllow();
			mScrollMatrix *= mat44<float>::translate(vec3<float>(_scroll.x, _scroll.y, 0) - mScrollPrev);
			mScrollPrev = vec3<float>(_scroll.x, _scroll.y, 0);
			this->on_coordinate_modify();
		}
	}


	void set_scroll_range_x(range<float> _range)
	{
		if (mScroller.range_x() != _range)
		{
			mScroller.set_range_x(_range);
			this->on_coordinate_modify();
		}
	}

	void set_scroll_range_y(range<float> _range)
	{
		if (mScroller.range_y() != _range)
		{
			mScroller.set_range_y(_range);
			this->on_coordinate_modify();
		}
	}

	range<float> scroll_range_x() const noexcept { return mScroller.range_x(); }
	range<float> scroll_range_y() const noexcept { return mScroller.range_y(); }

protected:
	// ========================================================================
	//! スクロール量が変化したときに呼ばれる。
	// ------------------------------------------------------------------------
	//! 引数の値を変更するとスクロール量を変更できます。
	// ------------------------------------------------------------------------
	virtual void on_scrolling(vec2<float>& _scroll) noexcept { }

	// ========================================================================
	//! スクロール量がオーバーフローしているときに呼ばれる。
	// ------------------------------------------------------------------------
	//! _reelase
	//!		マウスもしくはタッチパネルが離された時は true
	// ------------------------------------------------------------------------
	virtual void on_overfllow(vec2<float>& _overfllow, bool _release) noexcept { }

	virtual matrix_type children_matrix() const override
	{
		return	mScrollMatrix * super_type::children_matrix();
	}

	mat44<float>		mScrollMatrix			= mat44<float>::identify();
	vec3<float>			mScrollPrev				= {0, 0, 0};
	vec2<float>			mOverfllowPrev			= {0, 0};
	scroll				mScroller;
	bool				mEnableScroll			= true;
};


AMTRS_NAMESPACE_END
#endif
