/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__slider_node__hpp
#define	__libamtrs__node__slider_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN

template<class T, class NodeT>
struct	slider_node_traits
{
	using	value_type		= T;
	using	node_type		= typename NodeT::node_type;
};


//! スライダーを提供します。
//! ただし描画などは持ちません。バーを描画する場合は slider_bar_node を使用します。
template<class T, class NodeT, class Traits = slider_node_traits<T, NodeT>>
class	slider_node
		: public NodeT
{
public:
	using	super_type			= NodeT;
	using	node_type			= typename NodeT::node_type;
	using	traits_type			= Traits;
	using	value_type			= typename traits_type::value_type;
	using	trackbar_type		= fill_node<node_type>;


	slider_node(value_type _current, value_type _min, value_type _max)
		: mCurrent(_current)
		, mMin(_min)
		, mMax(_max)
	{}


	void set_value(value_type _pos)
	{
		if (mCurrent != _pos)
		{
			mCurrent = on_value_change(_pos);
		}
	}


	value_type get_current() const noexcept { return mCurrent; }


	double get_current_alpha() const noexcept { return static_cast<double>(mCurrent - mMin) / static_cast<double>(mMax - mMin); }


protected:
	//! 値が変更されたときに呼ばれるコールバック
	//! 戻り値で返した値が最終的な値として使用される。
	virtual value_type on_value_change(value_type _value)
	{
		return	_value;
	}

	virtual value_type on_input_release(value_type _value)
	{
		return	_value;
	}

	virtual node* on_input(typename super_type::pointer_event* _e) override
	{
		if (!mInputCapture)
		{
			if (auto r = super_type::on_input(_e); r)
			{
				return	r;
			}
		}

		auto*	event	= _e->event()->get_mouse_event();
		if (event)
		{
			auto	mat			= this->world_matrix().inverse();
			auto	pos			= vec2<float>(vec3<float>(event->template get_point<float>(), 0) * mat);
			float	alpha		= pos.x / this->size().width;
			if (alpha < 0)
			{
				alpha	= 0;
			}
			if (alpha > 1)
			{
				alpha	= 1;
			}
			auto	value		= static_cast<value_type>(((float)(mMax - mMin) * alpha)) + mMin;
			if ((event->state == input_event::press) && (event->buttons == (1<<0)))
			{
				// スイッチの位置を割り出す
				if (this->size().is_inner(pos))
				{
					mPressing			= true;
					mDragging			= false;
					mStartPos			= pos;
					set_value(value);
				}
			}
			else if ((event->state == input_event::drag) && mPressing)
			{
				if (this->size().is_inner(pos))
				{
					if ((std::abs(pos.x - mStartPos.x) >= 0) && (std::abs(pos.x - mStartPos.x) > std::abs(pos.y - mStartPos.y)))
					{
						mDragging	= true;
					}
				}
				if (mDragging)
				{
					mInputCapture		= true;
					set_value(value);
				}
			}
			else if (event->state == input_event::release)
			{
				mInputCapture	= false;
				if (mPressing)
				{
					mPressing		= false;
					set_value(on_input_release(value));
				}
			}
		}
		return	mInputCapture ? this : nullptr;
	}

private:
	value_type											mCurrent;
	value_type											mMin;
	value_type											mMax;

	vec2<float>											mStartPos;
	bool												mPressing		= false;
	bool												mDragging		= false;
	bool												mInputCapture	= false;
};



AMTRS_NAMESPACE_END
#endif
