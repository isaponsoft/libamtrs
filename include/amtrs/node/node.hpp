/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__node__node__hpp
#define	__libamtrs__node__node__hpp
#include <stack>
#include "../amtrs.hpp"
#include "../graphics/@"
#include "../input/@"
#include "../string/@"
#include "@fwd.hpp"
#include "action.hpp"
#include "node_traits.hpp"
#include "component/action_component.hpp"
#include "component/color_component.hpp"
#include "component/hierarchy_component.hpp"
#include "component/input_component.hpp"
#include "component/name_component.hpp"
#include "component/renderer_component.hpp"
#include "component/scheduler_component.hpp"
#include "component/transform_component.hpp"
#include "component/user_data_component.hpp"
AMTRS_NAMESPACE_BEGIN



template<class T, class Traits>
class	basic_node<T, Traits, void>
		: public ref_object
		, public component::action_component	<basic_node<T, Traits>>
		, public component::color_component		<basic_node<T, Traits>>
		, public component::hierarchy_component	<basic_node<T, Traits>>
		, public component::input_component		<basic_node<T, Traits>>
		, public component::name_component		<std::string, basic_node<T, Traits>>
		, public component::renderer_component	<Traits>
		, public component::scheduler_component	<basic_node<T, Traits>, void(float)>
		, public component::transform_component	<T, Traits>
		, public component::user_data_component	<basic_node<T, Traits>, void*>
{
public:
	using	node_type					= basic_node<T, Traits, void>;
	using	traits_type					= Traits;
	using	action_type					= basic_action;

	using	transform_component			= typename component::transform_component<T, Traits>;
	using	box_type					= typename transform_component::box_type;
	using	position_type				= typename transform_component::position_type;

	using	name_component				= typename component::name_component<std::string, node_type>;

	using	hierarcy_component			= typename component::hierarchy_component<node_type>;
	using	on_child_event				= typename hierarcy_component::on_child_event;

	using	renderer_component			= typename component::renderer_component<traits_type>;
	using	scheduler_component			= typename component::scheduler_component<node_type, void(float)>;

	using	schede_dispatcher_type		= typename scheduler_component::schede_dispatcher_type;

	using	layer						= typename renderer_component::layer;
	using	render_queue				= typename renderer_component::render_queue;


	// ========================================================================
	//! 子ノードを覆うバウンディングボックスを計算します。
	// ------------------------------------------------------------------------
	box_type children_bounds() const noexcept
	{
		box_type	bounds	= {{0, 0, 0}, {0, 0, 0}};
		this->visit_children([&](auto c) -> bool
		{
			auto		br	= box_type(c->position() - (position_type(c->size()) * c->pivot()), c->size() * c->scale());
			if (bounds.left() > br.left())
			{
				bounds.x	= br.x;
			}
			if (bounds.top() > br.top())
			{
				bounds.y	= br.y;
			}
			if (bounds.front() > br.front())
			{
				bounds.z	= br.z;
			}
			if (bounds.left() > br.left())
			{
				bounds.x	= br.x;
			}
			if (bounds.right() < br.right())
			{
				bounds.width	=  br.right() - bounds.left();
			}
			if (bounds.bottom() < br.bottom())
			{
				bounds.height	=  br.bottom() - bounds.top();
			}
			if (bounds.back() < br.back())
			{
				bounds.depth	=  br.back() - bounds.front();
			}
			return	false;
		});
		return	bounds;
	}

	// ========================================================================
	//! 子ノードを覆うサイズに自信のサイズを設定します。
	// ------------------------------------------------------------------------
	node_type* set_size_by_children()
	{
		this->set_size(this->children_bounds().size());
		return	this;
	}



protected:

	virtual void on_created() override
	{
		scheduler_component::on_created();
		auto	s	= this->size();
		this->on_size_change(s);
	}



	// ========================================================================
	//! 自身または親ノードの可視状態が変更したことを通知します。
	// ------------------------------------------------------------------------
	virtual void on_visibility(bool _visible)
	{
		this->visit_children([_visible](auto* c) -> bool
		{
			c->on_visibility(_visible);
			return	false;
		});
	}

	virtual void on_child(node_type* _child, on_child_event _event) override
	{
		hierarcy_component::on_child(_child, _event);
		this->rerender_request();
		_child->set_parent_transform(_event == on_child_event::add ? this : nullptr);

		// ルートスケジューラの設定
		if (_event == on_child_event::add)
		{
			if (auto* rs = this->get_root_scheduler(); rs)
			{
				// ルートスケジューラを持っている場合は子を再帰的的にスケジューラを設定します。
				struct	_
				{
					static void set(node_type* _n, schede_dispatcher_type* _s)
					{
						_n->set_root_scheduler(_s);
						_n->on_enter();
						_n->visit_children([&](auto c) -> bool
						{
							set(c, _s);
							return	false;
						});
					}
				};
				_::set(_child, rs);
			}
		}
		if (_event == on_child_event::remove)
		{
			if (auto* rs = this->get_root_scheduler(); rs)
			{
				// ルートスケジューラを持っている場合は子から再帰的的にスケジューラを外します。
				struct	_
				{
					static void set(node_type* _n)
					{
						_n->visit_children([&](auto c) -> bool
						{
							set(c);
							return	false;
						}, false);
						if (_n->get_root_scheduler())
						{
							_n->on_exit();
							_n->set_root_scheduler(nullptr);
						}
					}
				};
				_::set(_child);
			}
		}
	}


	virtual void on_enter()
	{}


	virtual void on_exit()
	{}


	virtual void on_render(render_queue& _rq) override
	{
		renderer_component::on_render(_rq);
		layer*	current	= _rq.current;
		this->visit_children([&](auto* c) -> bool
		{
			if (c->visibility() && c->is_enable())
			{
				if (auto* cl = c->get_layer(); cl)
				{
					_rq.current	= cl;
				}
				c->on_render(_rq);
				_rq.current	= current;
			}
			return	false;
		});
	}


	virtual void on_chnage_color() noexcept override
	{
		this->rerender_request();
	}


	virtual void on_coordinate_modify() noexcept override
	{
		transform_component::on_coordinate_modify();
		this->visit_children([](auto* c) -> bool
		{
			c->on_coordinate_modify();
			return	false;
		});
		this->rerender_request();
	}
};




using	node	= amtrs::basic_node<float>;


AMTRS_NAMESPACE_END
#endif
