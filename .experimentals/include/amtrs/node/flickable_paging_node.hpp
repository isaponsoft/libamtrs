/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__flickable_paging_node__hpp
#define	__libamtrs__node__flickable_paging_node__hpp
#include "paging_node.hpp"
#include "updatable_node.hpp"
#include "ui_utils/scroll.hpp"
AMTRS_NAMESPACE_BEGIN

struct	flickable_paging_node_base
{
	static constexpr unsigned int	axis_x	= uiutils::scroll::axis_x;
	static constexpr unsigned int	axis_y	= uiutils::scroll::axis_y;
};

template<class SuperT, class Traits = paging_node_traits<SuperT>>
class	flickable_paging_node
		: public updatable_node<paging_node<SuperT, Traits>>
		, public flickable_paging_node_base
{
public:
	using	super_type		= updatable_node<paging_node<SuperT, Traits>>;
	using	node_type		= typename super_type::node_type;
	using	page_index_type	= typename super_type::page_index_type;


	using	super_type::super_type;

	void set_scroll_axis(uint32_t _axis = axis_x|axis_y)
	{
		mScroller.set_axis(_axis);
	}

	void set_input_enable(bool _enable) noexcept { mInputEnable = _enable; }

protected:
	virtual void on_input_cancel() override
	{
		super_type::on_input_cancel();
		mScroller.cancel(*this);
	}

	virtual node_type* on_input(typename super_type::pointer_event* _e) override
	{
		if (auto* mouse = _e->event()->get_mouse_event(); mouse && mInputEnable)
		{
			auto	mat		= this->world_matrix().inverse();
			auto	pos		= vec2<float>(vec3<float>(mouse->template get_point<float>(),0) * mat);
			if (mScroller.is_capturing())
			{
				if (mouse->type == input_event::mouse)
				{
					mScroller.input(*this, mouse, &pos);
				}
				if (mouse->state == input_event::status::release)
				{
					auto	spos	= mScroller.position();
					auto	trans	= this->compute_page_position(this->active_page()) + vec3<float>(-spos.x, -spos.y, 0);
					bool	change	= false;
					if (spos.x < -this->size().width /3) { change = true; trans.x += this->size().width/2; }
					if (spos.x >  this->size().width /3) { change = true; }
					if (spos.y < -this->size().height/3) { change = true; trans.x += this->size().height/2; }
					if (spos.y >  this->size().height/3) { change = true; }

					if (change)
					{
						auto	pagepos	= this->compute_page_index(trans);
						this->set_active_page(pagepos, false);
						mScroller.set_position({0, 0});
					}
				}
				return	this;
			}
			if (auto r = super_type::on_input(_e); r)
			{
				return	r;
			}
			if (mouse->type == input_event::mouse)
			{
				mScroller.input(*this, mouse, &pos);
			}
			return	nullptr;
		}
		return	super_type::on_input(_e);
	}

	


	virtual void on_update(float _delta) override
	{
		super_type::on_update(_delta);
		mScroller.update(_delta);
		auto	now	= vec3<float>(mScroller.position());
		// スクロール位置が変化しているならページが切り替わったかどうかを検出する
		if (mScrollPrev != now)
		{
			mScrollPrev	= now;


			auto		pgidx	= this->active_page();						// 現在のページ位置
			vec3<float>	cldpos	= this->compute_children_translate(pgidx);	// 現在のページ位置に対するスクロール位置
			vec3<float>	scroll	= {0, 0, 0};
			scroll.x	= cldpos.x + mScrollPrev.x;
			scroll.y	= cldpos.y + mScrollPrev.y;
			super_type::set_children_position(scroll);
		}
	}

private:
	uiutils::scroll	mScroller;
	vec3<float>		mScrollPrev		= {0, 0, 0};
	bool			mInputEnable	= true;
};

AMTRS_NAMESPACE_END
#endif
