/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__list_node__hpp
#define	__libamtrs__node__list_node__hpp
#include "itemable_node.hpp"
#include "scroll_node.hpp"
AMTRS_NAMESPACE_BEGIN


template<class BaseT>
struct	list_node_traits
{
	using	base_type	= BaseT;
};


// ****************************************************************************
//! 子に追加されたアイテムをリストにして整列します。
// ----------------------------------------------------------------------------
//! add_child() を呼んだ場合は、通常の node と同じように扱われるため、アイテム
//! としては扱われなくなるため、add_node() は隠蔽されます。
//! 
//! しかし、通常のノードとして意図的に add_child() を呼び出したい場合は
//! node_type* にキャストして呼び出すことができます。
// ----------------------------------------------------------------------------
template<class NodeT, class Traits = list_node_traits<NodeT>>
class	list_node						// vertical_list_node
		: public itemable_node<typename Traits::base_type>
{
public:
	using	super_type		= itemable_node<typename Traits::base_type>;
	using	node_type		= typename super_type::node_type;
	using	size_type		= typename super_type::size_type;
	using	item_type		= typename NodeT::node_type;
	using	containor_type	= const std::vector<item_type*>;

	using	super_type::super_type;


	bool auto_resize() const noexcept { return mAutoResize; }

	// ========================================================================
	//! このノードの自動リサイズを有効にします。
	// ------------------------------------------------------------------------
	//! 自動リサイズを有効にすると、align_items() によってアイテムが整列された
	//! ときに自動的に自身のサイズをアイテムの大きさに合わせてリサイズします。
	// ------------------------------------------------------------------------
	void set_auto_resize(bool _enable) noexcept
	{
		if (mAutoResize != _enable)
		{
			mAutoResize	= _enable;
			align_items();
		}
	}


	// ========================================================================
	//! アイテムを再度整列します。
	// ------------------------------------------------------------------------
	void align_items()
	{
		update_bounds();
		if (mAutoResize)
		{
			this->on_auto_size(mItemsBounds);
		}
	}


	// ========================================================================
	//! アイテムの配置されている範囲を取得します。
	// ------------------------------------------------------------------------
	size_type get_items_bounds() const noexcept { return mItemsBounds; }

	void set_spacing(float _spacing)
	{
		mSpacing	= _spacing;
		align_items();
	}

	float spacing() const noexcept
	{
		return	mSpacing;
	}

protected:
	virtual void on_child(node_type* _child, typename super_type::on_child_event _event) override
	{
		super_type::on_child(_child, _event);
		resort();
		align_items();
	}

	virtual void on_children_transform_change(typename super_type::transform_component* _child) override
	{
		super_type::on_children_transform_change(_child);
		mItemsBounds	= super_type::items_bounds().size();
		if (mAutoResize)
		{
			this->on_auto_size(mItemsBounds);
		}
	}

	virtual void on_children_size_change(typename super_type::transform_component* _child) override
	{
		super_type::on_children_size_change(_child);
		align_items();
	}

	virtual void on_size_change(size_type& _size) override
	{
		super_type::on_size_change(_size);
		update_bounds();	// 無限ループしないように、align_items() は呼ばない。
	}

	virtual void on_change_children_order() override
	{
		super_type::on_change_children_order();
		resort();
		update_bounds();
	}

	virtual void on_auto_size(size_type& _size)
	{
		this->set_size(_size);
	}

	// ========================================================================
	//! アイテムの位置情報を変更する必要があることを通知します。
	// ------------------------------------------------------------------------
	//! 派生クラスでオーバーライドし、アイテムを適切な位置とサイズに
	//! 調整してください。
	// ------------------------------------------------------------------------
	virtual void on_reposition(containor_type& _items, const size_type& _size) = 0;

private:
	void update_bounds()
	{
		on_reposition(super_type::items(), this->size());
	}

	void resort()
	{
		std::sort(super_type::items().begin(), super_type::items().end(), [](auto a, auto b) -> bool
		{
			return	a->priority() < b->priority();
		});
	}

	bool						mAutoResize			= false;
	size_type					mItemsBounds		= {0, 0, 0};
	float						mSpacing			= 0;
};


AMTRS_NAMESPACE_END
#endif
