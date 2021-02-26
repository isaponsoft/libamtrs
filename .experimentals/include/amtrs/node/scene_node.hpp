/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__scene_node__hpp
#define	__libamtrs__node__scene_node__hpp
#include <stack>
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN

template<class BaseNodeT>
class	scene_node_stack;


template<class BaseNodeT>
class	scene_node_if
{
public:
	using	node_type			= BaseNodeT;
	using	traits_type			= typename BaseNodeT::traits_type;
	using	layer_type			= typename BaseNodeT::layer;
	using	draw_argument		= typename traits_type::draw_argument;
	using	renderer_argument	= typename BaseNodeT::renderer_argument;
	using	render_queue		= typename BaseNodeT::render_queue;
	using	scene_stack_type	= scene_node_stack<node_type>;


	layer_type* add_layer(layer_type* _layer)
	{
		mSortLayer	= true;
		mLayerList.push_back(_layer);
		return	_layer;
	}

	layer_type* find_layer(const std::string& _name) const noexcept
	{
		auto	it	= std::find_if(mLayerList.begin(), mLayerList.end(), [&](auto a) -> bool
					{
						return	a->name() == _name;
					});
		return it == mLayerList.end() ? nullptr : *it;
	}

	virtual void render(renderer_argument& _rarg) {}
	virtual node_type* input(input_event* _event) { return nullptr; }


	virtual void draw_exec(draw_argument _darg, bool _remove_queue = true)
	{
		for (auto& l : bucket())
		{
			l->on_transfer(_darg);
		}
		for (auto& l : bucket())
		{
			l->on_draw(_darg);
		}
		if (_remove_queue)
		{
			draw_clear();
		}
	}

	void draw_clear()
	{
		for (auto& l : bucket())
		{
			l->clear();
		}
	}


protected:
	virtual void on_scene_enter() = 0;
	virtual void on_scene_exit() = 0;
	virtual void on_visible() { mVisibled = true; }
	virtual void on_active() { mActivated = true; }
	virtual void on_deactive() { mActivated = false; }
	virtual void on_invisible() { mVisibled = false; }


	scene_stack_type* get_scene_stack() const noexcept { return mSceneStack; }

	auto bucket()
	{
		if (mSortLayer)
		{
			mSortLayer	= false;
			std::sort(mLayerList.begin(), mLayerList.end(), [](auto& a, auto& b) -> bool
			{
				return	a->get_layer_order() <= b->get_layer_order();
			});
		}
		return	make_bucket(mLayerList.begin(), mLayerList.end());
	}


private:
	void enter()
	{
		this->on_scene_enter();
	}

	void exit()
	{
		this->on_scene_exit();
	}


	std::vector<layer_type*>	mLayerList;
	scene_stack_type*			mSceneStack	= nullptr;
	bool						mSortLayer	= false;
	bool						mEnter		= false;
	bool						mPop		= false;
	bool						mVisibled	= false;
	bool						mActivated	= false;

	template<class>
	friend	class	scene_node_stack;
};


template<class NodeT>
class	scene_node
		: public NodeT
		, public scene_node_if<typename NodeT::node_type>
		, protected NodeT::layer
		, protected component::input_dispatcher<typename NodeT::node_type>
{
	using	_base_type				= NodeT;
	using	_if_type				= scene_node_if<typename NodeT::node_type>;
	using	_input_dispacher		= component::input_dispatcher<typename NodeT::node_type>;
public:
	using	node_type				= typename _base_type::node_type;
	using	traits_type				= typename _base_type::traits_type;
	using	layer_type				= typename _base_type::layer;
	using	draw_argument			= typename traits_type::draw_argument;
	using	renderer_argument		= typename _base_type::renderer_argument;
	using	render_queue			= typename _base_type::render_queue;
	using	schede_dispatcher_type	= typename _base_type::schede_dispatcher_type;

	using	_base_type::_base_type;

	scene_node()
	{
		_if_type::add_layer(this);
	}


	// 描画
	virtual void render(renderer_argument& _rarg) override
	{
		this->on_render(_rarg);
	}

	virtual node_type* input(input_event* _event) override
	{
		return	_input_dispacher::input(this, _event);
	}

	virtual void on_scene_enter() override
	{
	}

	virtual void on_scene_exit() override
	{
	}

private:
	ref<schede_dispatcher_type>	mRootScheduler;
};



AMTRS_NAMESPACE_END
#endif
