/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__vertical_list_node__hpp
#define	__libamtrs__node__vertical_list_node__hpp
#include "list_node.hpp"
AMTRS_NAMESPACE_BEGIN



// ****************************************************************************
//! 子に追加されたアイテムをリストにして縦に整列します。
// ----------------------------------------------------------------------------
template<class NodeT, class Traits = list_node_traits<NodeT>>
class	vertical_list_node
		: public list_node<NodeT, Traits>
{
public:
	using	position_callback_type	= vec3<float>(typename NodeT::node_type*, vec3<float>);

	using	super_type				= list_node<NodeT, Traits>;

	// ========================================================================
	//! アイテムの配置位置を示す。
	// ------------------------------------------------------------------------
	enum class	valign_type
	{
		top,					//!< 上に配置
		bottom					//!< 下方向に配置
	};

	using	containor_type	= typename super_type::containor_type;

	using	super_type::super_type;

	valign_type valign() const noexcept { return mAlignType; }

	void set_valign(valign_type _type) { if (mAlignType != _type) { mAlignType = _type; update_scroll_range(); } }

	void set_list_item_position(std::function<position_callback_type> _callback, std::function<position_callback_type> _next_callback = {})
	{
		mPositionCallback		= std::move(_callback);
		mNextPositionCallback	= std::move(_next_callback);
	}

protected:
	//! 再配置を行います。
	virtual void on_reposition(containor_type& _items, const typename super_type::size_type& _size) override
	{
		vec3<float>	pos		= {0, 0, 0};
		for (auto* item : _items)
		{
			vec3<float>	np(item->position().x, pos.y, item->position().z);
			if (mPositionCallback)
			{
				np	= mPositionCallback(item, np);
			}
			item->set_position(np);
			pos.y	+= item->size().height + super_type::spacing();
			if (mNextPositionCallback)
			{
				pos	= mNextPositionCallback(item, pos);
			}
		}
	}


	virtual void on_child(typename super_type::node_type* _child, typename super_type::on_child_event _event) override
	{
		super_type::on_child(_child, _event);
		update_scroll_range();
	}


	virtual void on_children_transform_change(typename super_type::transform_component* _child) override
	{
		super_type::on_children_transform_change(_child);
		update_scroll_range();
	}


	virtual void on_size_change(typename NodeT::size_type& _size) override
	{
		super_type::on_size_change(_size);
		update_scroll_range();
	}

	virtual void on_auto_size(typename NodeT::size_type& _size) override
	{
		this->set_height(_size.height);
	}

	void update_scroll_range()
	{
		if constexpr (std::is_base_of<scroll_node<NodeT>, super_type>::value)
		{
			auto	bounds		= this->get_items_bounds();
			auto	size		= this->size();
			auto	diff		= size.height - bounds.height;
			auto	sr			= range<float>({std::min<float>(0, diff), 0});	// top
			if (mAlignType == valign_type::bottom)
			{
				sr = range<float>({
					size.height - bounds.height,
					0//std::max<float>(size.height - bounds.height, bounds.height - size.height)
				});
			}
			this->set_scroll_range_y(sr);
		}
	}

private:
	valign_type								mAlignType		= valign_type::top;
	std::function<position_callback_type>	mPositionCallback;
	std::function<position_callback_type>	mNextPositionCallback;
};


AMTRS_NAMESPACE_END
#endif
