/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__slider_bar_node__hpp
#define	__libamtrs__node__slider_bar_node__hpp
#include "fill_node.hpp"
#include "slider_node.hpp"
AMTRS_NAMESPACE_BEGIN


//! スライダーバーを提供します。
template<class T, class NodeT, class Traits = slider_node_traits<T, NodeT>>
class	slider_bar_node
		: public slider_node<T, NodeT, Traits>
{
public:
	using	super_type	= slider_node<T, NodeT, Traits>;
	using	node_type	= typename super_type::node_type;
	using	value_type	= T;

	template<class... Args>
	slider_bar_node(Args&&... _args)
		: super_type(std::forward<Args>(_args)...)
	{
		mBarBg	= new fill_node<node_type>();
		mBarBg->set_color({0.4f, 0.4f, 0.4f, 1});
		mBarBg->set_pivot({0, 0.5f, 0});
		this->add_child(mBarBg);

		mBar	= new fill_node<node_type>();
		mBar->set_color({0.8f, 0.8f, 0.8f, 1});
		mBar->set_pivot({0, 0.5f, 0});
		this->add_child(mBar);

		this->schedule_once([this](float)
		{
			this->update_position();
		});
	}

	void set_color(rgba<float> _active, rgba<float> _bg)
	{
		mBarBg->set_color(_bg);
		mBar->set_color(_active);
	}

	node_type* get_track_button()
	{
		return	mButton;
	}

	template<class Child>
	Child* set_track_button(Child* _button)
	{
		static_assert(std::is_base_of<node_type, Child>::value);
		_set_track_button(_button);
		return	_button;
	}


	void update_position()
	{
		if (mButton)
		{
			mButton->set_position({(float)super_type::get_current_alpha() * this->size().width, this->size().height/2, 0});
		}
		mBarBg->set_size({this->size().width                                         , mHeight, 0});
		mBar->  set_size({this->size().width * (float)super_type::get_current_alpha(), mHeight, 0});
		mBarBg->set_position({0, this->size().height/2, 0});
		mBar->  set_position({0, this->size().height/2, 0});
	}

	void set_value_change_listener(std::function<value_type(value_type)> _listener)
	{
		mListener	= std::move(_listener);
	}

protected:
	virtual value_type on_value_change(value_type _value) override
	{
		_value	= super_type::on_value_change(_value);
		if (mListener)
		{
			_value	= mListener(_value);
		}
		this->schedule_once([this](float)
		{
			this->update_position();
		});
		return	_value;
	}


	virtual void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
		update_position();
	}


	void _set_track_button(node_type* _button)
	{
		if (mButton == _button)
		{
			return;
		}

		if (mButton)
		{
			mButton->remove_from_parent();
		}
		mButton = _button;
		if (mButton)
		{
			this->add_child(mButton);
		}

		update_position();
	}


	node_type*								mButton		= nullptr;
	fill_node<node_type>*					mBarBg		= nullptr;
	fill_node<node_type>*					mBar		= nullptr;
	float									mHeight		= 4;
	std::function<value_type(value_type)>	mListener;
};

AMTRS_NAMESPACE_END
#endif
