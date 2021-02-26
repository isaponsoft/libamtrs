/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__itemable_node__hpp
#define	__libamtrs__node__itemable_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(itemable_node)
template<class NodeT>
struct itemable_node_valid
{};

template<class SuperT, class Traits>
class itemable_node_impl
		: public SuperT
		, public itemable_node_valid<typename SuperT::node_type>
{
public:
	using	super_type		= SuperT;
	using	node_type		= typename super_type::node_type;
	using	use_scheduler	= typename super_type::use_scheduler;
	using	position_type	= typename super_type::position_type;
	using	box_type		= typename super_type::box_type;
	using	on_child_event	= typename super_type::on_child_event;
	using	traits_type		= Traits;

	using	super_type::super_type;


	// ========================================================================
	//! アイテムを追加します。
	// ------------------------------------------------------------------------
	template<class T> T add_item(T _item)
	{
		static_assert(std::is_convertible<T, node_type*>::value);
		_add_item(static_cast<node_type*>(_item));
		return	_item;
	}

	// ========================================================================
	//! スケジューラを使用してアイテムを追加します。
	// ------------------------------------------------------------------------
	//! メインスレッド以外からadd_itemを呼び出したい場合に使用します。
	//! node->schedule_once([itemnode, item]() { itemnode->add_item(item); });
	//! と同等です。
	// ------------------------------------------------------------------------
	template<class T, class Option> T add_item(T _item, Option _option)
	{
		static_assert(std::is_convertible<T, node_type*>::value);
		if constexpr (std::is_same<Option, use_scheduler>::value)
		{
			ref<node_type>	i(_item);
			this->schedule_once([this, i]()
			{
				_add_item(static_cast<node_type*>(i));
			});
			return	_item;
		}
	}


	auto const& items() const noexcept { return mItems; }
	auto const& citems() const noexcept { return mItems; }

protected:
	auto& items() noexcept { return mItems; }

	box_type items_bounds() const noexcept
	{
		auto*	node	= static_cast<node_type const*>(this);
		if (mItems.empty())
		{
			return	{{0,0,0}, {0,0,0}};
		}


		box_type	bounds	= {node->child(0)->position(), node->child(0)->size()};
		for (auto& c : mItems)
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
		}
		return	bounds;
	}

	virtual void on_child(node_type* _child, on_child_event _event) override
	{
		super_type::on_child(_child, _event);
		if (_event == super_type::on_child_event::remove)
		{
			if (auto it = std::find(mItems.begin(), mItems.end(), _child); it != mItems.end())
			{
				mItems.erase(it);
			}
		}
	}

	virtual void on_change_children_order() override
	{
		super_type::on_change_children_order();
		std::sort(mItems.begin(), mItems.end(), [](auto& a, auto& b) { return a->priority() < b->priority(); });
	}


private:
	void _add_item(node_type* _item)
	{
		mItems.push_back(_item);
		super_type::add_child(_item);
		traits_type{}.add_item(_item, this);
	}


	std::vector<node_type*>	mItems;
};
AMTRS_IMPLEMENTS_END(itemable_node)

// ****************************************************************************
//! アイテムを持つノード
// ----------------------------------------------------------------------------
//! アイテムとは子ノードの扱いの一つです。リストタイプのノードなどはアイテムを
//! 自動的に整理するなど、親ノードによる管理を行うことを前提とした子ノードと
//! なります。
// ----------------------------------------------------------------------------
template<class SuperT, class Traits = itemable_node_traits<SuperT>>
using	itemable_node	=
			typename std::conditional<
				std::is_base_of<AMTRS_IMPLEMENTS(itemable_node)::itemable_node_valid<typename SuperT::node_type>, SuperT>::value,
				SuperT,															
				AMTRS_IMPLEMENTS(itemable_node)::itemable_node_impl<SuperT, Traits>
			>::type;







template<class SuperT, class... Args>
struct	itemable_node_traits
{
	template<class N, class This>
	void add_item(N* _node, This* _this)
	{
	}
};

AMTRS_NAMESPACE_END
#endif
