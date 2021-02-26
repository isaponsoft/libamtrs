/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__node__hpp
#define	__libamtrs__node__node__hpp
#include "amtrs.hpp"
#include "g3d.hpp"
#include "graphics.hpp"
#include "input.hpp"
#include "string.hpp"
#include "utility/message.hpp"
#include "utility/singleton.hpp"
#include "mutex/safe_spinlock.hpp"
#include "type_traits/can_invoke.hpp"


AMTRS_NAMESPACE_BEGIN
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
#include ".inc/node-action.hpp"
#include ".inc/node-node_traits.hpp"
#include ".inc/node-action_component.hpp"
#include ".inc/node-color_component.hpp"
#include ".inc/node-hierarchy_component.hpp"
#include ".inc/node-input_component.hpp"
#include ".inc/node-name_component.hpp"
#include ".inc/node-renderer_component.hpp"
#include ".inc/node-scheduler_component.hpp"
#include ".inc/node-thread_component.hpp"
#include ".inc/node-transform_component.hpp"
#include ".inc/node-user_data_component.hpp"
AMTRS_NAMESPACE_BEGIN



template<class T, class Traits>
class	basic_node<T, Traits, void>
		: public ref_object
		, public component::scheduler_component	<basic_node<T, Traits>, void(float)>	// it's first initialize
		, public component::action_component	<basic_node<T, Traits>>
		, public component::color_component		<basic_node<T, Traits>>
		, public component::hierarchy_component	<basic_node<T, Traits>>
		, public component::input_component		<basic_node<T, Traits>>
		, public component::name_component		<basic_node<T, Traits>>
		, public component::renderer_component	<basic_node<T, Traits>>
		, public component::thread_component	<basic_node<T, Traits>>
		, public component::transform_component	<basic_node<T, Traits>, T, Traits>
		, public component::user_data_component	<basic_node<T, Traits>, void*>
{
public:
	using	node_type					= basic_node<T, Traits, void>;
	using	traits_type					= Traits;
	using	action_type					= basic_action;
	using	hierarcy_component			= typename component::hierarchy_component<node_type>;
	using	renderer_component			= typename component::renderer_component<node_type>;
	using	on_child_event				= typename hierarcy_component::on_child_event;

	//! add_child などでスケジューラを使用するように指定するときに使用します。
	class	use_scheduler
	{
	public:
		use_scheduler() {}
	};



	virtual auto insert_child(node_type* _pos, ref<node_type> _child) -> node_type* override
	{
		node_type*	c	= _child.get();
		hierarcy_component::insert_child(_pos, std::move(_child));
		this->rerender_request();
		c->set_parent_transform(this);
		c->set_scheduler(this->get_scheduler());
//		c->clear_color_cache();
		this->rerender_request();
		return	c;
	}

	virtual bool remove_child(node_type* _child) override
	{
		ref<node_type>	keep(_child);
		if (!hierarcy_component::remove_child(_child))
		{
			return	false;
		}
		keep->set_parent_transform(nullptr);
		keep->set_scheduler(nullptr);
		this->rerender_request();
		return	true;
	}

protected:
	virtual void on_coordinate_modify() override
	{
		using	transform_component			= typename component::transform_component<node_type, T, Traits>;
		transform_component::on_coordinate_modify();
		renderer_component::rerender_request();
	}
};






// ============================================================================
//! アルファ値を変更します。
// ----------------------------------------------------------------------------
//!	example)
//!		act = new alpha_by_action<node, node::action_type>(
//!			1.0f,		// to alpha.
//!			1.0f		// duration time.
//!		);
// ----------------------------------------------------------------------------
template<class NodeT, class TweenT = waveform::linear<float>>
class	alpha_by_action
		: public basic_tween_action<TweenT>
{
public:
	using	super_type	= basic_tween_action<TweenT>;
	using	value_type	= float;
	using	node_type	= NodeT;

	template<class... TweenArgs>
	alpha_by_action(value_type _alpha, float _duration, TweenArgs&&... _targs)
		: super_type(_duration, std::forward<TweenArgs>(_targs)...)
		, mEnd(_alpha)
	{}

	virtual void on_start() override
	{
		super_type::on_start();
		mStart	= static_cast<node_type*>(this->target())->get_color().a;
	}


	virtual void on_update(float _time) override
	{
		super_type::on_update(_time);
		auto	v		= (mStart * (1.0f - _time)) + (mEnd * _time);
		auto	c		= static_cast<node_type*>(this->target())->get_color();
		c.a	= v;
		static_cast<node_type*>(this->target())->set_color(c);
	}

protected:
	value_type	mStart;
	value_type	mEnd;
};


AMTRS_NAMESPACE_END
#endif
