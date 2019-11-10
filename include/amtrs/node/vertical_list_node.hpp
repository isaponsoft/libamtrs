#ifndef	__libamtrs__node__vertical_list_node__hpp
#define	__libamtrs__node__vertical_list_node__hpp
#include "list_node.hpp"
AMTRS_NAMESPACE_BEGIN



// ****************************************************************************
//! 子に追加されたアイテムをリストにして縦に整列します。
// ----------------------------------------------------------------------------
//! add_child() の代わりに add_item() を使い、ノードを追加すると
//! 追加されたノードは vertical_list_node と同じ横幅にサイズが調整され
//! 縦に並べられます。
// ----------------------------------------------------------------------------
template<class NodeT, class ItemT = typename NodeT::node_type>
class	vertical_list_node
		: public list_node<NodeT, ItemT>
{
	using	_base_type	= list_node<NodeT, ItemT>;
public:
	// ========================================================================
	//! アイテムの配置位置を示す。
	// ------------------------------------------------------------------------
	enum class	valign_type
	{
		top,					//!< 上に配置
		bottom					//!< 下方向に配置
	};

	using	containor_type	= typename _base_type::containor_type;

	using	_base_type::_base_type;

	valign_type valign() const noexcept { return mAlignType; }

	void set_valign(valign_type _type) { if (mAlignType != _type) { mAlignType = _type; update_scroll_range(); } }

protected:
	//! 再配置を行います。
	virtual void on_reposition(containor_type& _items, const typename _base_type::size_type& _size) override
	{
		vec2<float>	pos		= {0, 0};
		for (auto* item : _items)
		{
			item->set_position({pos.x, pos.y, item->position().z});
			pos.y	+= item->size().height;
		}
	}


	virtual void on_child(typename _base_type::node_type* _child, typename _base_type::on_child_event _event) override
	{
		_base_type::on_child(_child, _event);
		if (_event == _base_type::on_child_event::add)
		{
			_child->set_width(this->size().width);
		}
		update_scroll_range();
	}


	virtual void on_children_transform_change(typename _base_type::transform_component* _child) override
	{
		_base_type::on_children_transform_change(_child);
		update_scroll_range();
	}


	virtual void on_size_change(typename NodeT::size_type& _size) override
	{
		_base_type::on_size_change(_size);
		for (auto* item : this->items())
		{
			item->set_width(_size.width);
		}
		update_scroll_range();
	}

	virtual void on_auto_size(typename NodeT::size_type& _size) override
	{
		this->set_height(_size.height);
	}

	void update_scroll_range()
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

private:
	valign_type		mAlignType	= valign_type::top;
};


AMTRS_NAMESPACE_END
#endif
