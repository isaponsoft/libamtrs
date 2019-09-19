#ifndef	__libamtrs__node__node__hpp
#define	__libamtrs__node__node__hpp
#include <stack>
#include "../../amtrs.hpp"
#include "../../graphics.hpp"
#include "../../input.hpp"
#include "../../string.hpp"
#include "action.hpp"
#include "node_traits.hpp"
#include "component/action_component.hpp"
#include "component/color_component.hpp"
#include "component/director_component.hpp"
#include "component/hierarchy_component.hpp"
#include "component/input_component.hpp"
#include "component/name_component.hpp"
#include "component/renderer_component.hpp"
#include "component/scheduler_component.hpp"
#include "component/transform_component.hpp"
#include "component/uilayout_component.hpp"
#include "component/user_data_component.hpp"
AMTRS_NAMESPACE_BEGIN

template<class Traits>
class	basic_node;


template<class Traits>
class	basic_node
		: public ref_object
		, public component::action_component	<basic_node<Traits>>
		, public component::color_component		<basic_node<Traits>>
		, public component::director_component	<basic_node<Traits>, typename Traits::director>
		, public component::hierarchy_component	<basic_node<Traits>>
		, public component::input_component		<basic_node<Traits>>
		, public component::name_component		<basic_node<Traits>, std::string>
		, public component::renderer_component	<Traits>
		, public component::scheduler_component	<basic_node<Traits>, void(float)>
		, public component::transform_component	<basic_node<Traits>>
		, public component::uilayout_component	<basic_node<Traits>>
		, public component::user_data_component	<basic_node<Traits>, void*>
{
	using	director_component			= component::director_component<basic_node<Traits>, typename Traits::director>;
	using	transform_component			= component::transform_component<basic_node<Traits>>;
	using	update_scheduler_component	= component::scheduler_component<basic_node<Traits>, void(float)>;
	using	hierarcy_component			= component::hierarchy_component<basic_node<Traits>>;
public:
	using	node_type					= basic_node<Traits>;
	using	director_type				= typename director_component::director_type;
	using	schede_dispatcher_type		= typename update_scheduler_component::schede_dispatcher_type;
	using	action_type					= basic_action;
	using	layer						= typename component::renderer_component<Traits>::layer;
	using	render_queue				= typename component::renderer_component<Traits>::render_queue;
	using	on_child_event				= typename hierarcy_component::on_child_event;
	using	traits_type					= Traits;
	using	box_type					= typename transform_component::box_type;
	using	position_type				= typename transform_component::position_type;

	basic_node();
	~basic_node();



	// ========================================================================
	//! 描画レイヤーを取得します。
	// ------------------------------------------------------------------------
	layer* get_layer() const noexcept { return mLayer; }


	// ========================================================================
	//! 描画レイヤーを設定します。
	// ------------------------------------------------------------------------
	//! nullptr を設定した場合は親ノードに従います。
	// ------------------------------------------------------------------------
	void set_layer(layer* _layer) noexcept { mLayer = _layer; }


	// ========================================================================
	//! create時のコールバックを追加します。
	// ------------------------------------------------------------------------
	//! on_created() 
	// ------------------------------------------------------------------------
	template<class Callback>
	void add_created(Callback&& _callback)
	{
		this->schedule_once([cb = std::move(_callback)](float){ cb(); });
	}


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

	virtual bool visibility() const noexcept { return mVisible; }

	// ========================================================================
	//! 可視状態を変更します。
	// ------------------------------------------------------------------------
	//! 不可視状態ではレンダリングは行われませんが、ほかの機能は有効です。
	// ------------------------------------------------------------------------
	void set_visibility(bool _visible) noexcept { if (mVisible != _visible) { mVisible = _visible; on_visibility(mVisible); } }

protected:
	virtual void on_created()
	{
		mCreated	= true;
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
		this->rerender_request();
	}

private:
	// ************************************************************************
	// hierarchy_component
	// ************************************************************************
	virtual void on_parent(node_type* _parent) override
	{}


	layer*		mLayer		= nullptr;
	bool		mCreated	= false;
	bool		mVisible	= true;
};



template<class Traits>
basic_node<Traits>::basic_node()
	: transform_component(this)
{
	auto*	director	= this->get_default_director();
	if (director)
	{
		this->set_director(director);
	}

	this->add_created([this]()
	{
		this->on_created();
		auto	s	= this->size();
		this->on_size_change(s);
	});
}



template<class Traits>
basic_node<Traits>::~basic_node()
{}



AMTRS_NAMESPACE_END
#endif
