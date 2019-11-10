#ifndef	__libamtrs__node__list_node__hpp
#define	__libamtrs__node__list_node__hpp
#include "scroll_node.hpp"
AMTRS_NAMESPACE_BEGIN



// ****************************************************************************
//! 子に追加されたアイテムをリストにして整列します。
// ----------------------------------------------------------------------------
//! add_child() を呼んだ場合は、通常の node と同じように扱われるため、アイテム
//! としては扱われなくなるため、add_node() は隠蔽されます。
//! 
//! しかし、通常のノードとして意図的に add_child() を呼び出したい場合は
//! node_type* にキャストして呼び出すことができます。
// ----------------------------------------------------------------------------
template<class NodeT, class ItemT = typename NodeT::node_type>
class	list_node						// vertical_list_node
		: public scroll_node<NodeT>
{
	using	_base_type	= scroll_node<NodeT>;
	using	_base_type::add_child;					// No use.
	using	_base_type::insert_child;				// No use.

public:
	using	node_type		= typename _base_type::node_type;
	using	size_type		= typename _base_type::size_type;
	using	item_type		= ItemT;
	using	containor_type	= const std::vector<item_type*>;
	using	_base_type::_base_type;


	// ========================================================================
	//! アイテムを追加します。
	// ------------------------------------------------------------------------
	//! 追加されたアイテムは即座に整列されます。
	// ------------------------------------------------------------------------
	void add_item(item_type* _item)
	{
		mItems.push_back(_item);
		_base_type::add_child(_item);
	}


	// ========================================================================
	//! アイテムを追加します。
	// ------------------------------------------------------------------------
	//! 追加されたアイテムは即座に整列されます。
	// ------------------------------------------------------------------------
	void insert_item(item_type* _pos, item_type* _item)
	{
		if (_pos == nullptr)
		{
			mItems.push_back(_item);
		}
		else
		{
			mItems.insert(std::find(mItems.begin(), mItems.end(), _pos), _item);
		}
		_base_type::add_child(_item);
	}



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


	// ========================================================================
	//! アイテムの一覧を取得します。
	// ------------------------------------------------------------------------
	containor_type& items() const noexcept { return mItems; }

protected:
	virtual void on_child(node_type* _child, typename _base_type::on_child_event _event) override
	{
		_base_type::on_child(_child, _event);
		if (_event == _base_type::on_child_event::remove)
		{
			auto	it		= std::remove_if(mItems.begin(), mItems.end(), [_child](auto& a) -> bool
								{ return a == _child; }
							);
			if (it != mItems.end())
			{
				mItems.erase(it, mItems.end());
			}
		}
		align_items();
	}

	virtual void on_children_transform_change(typename _base_type::transform_component* _child) override
	{
		_base_type::on_children_transform_change(_child);
		align_items();
	}


	virtual void on_size_change(size_type& _size) override
	{
		_base_type::on_size_change(_size);
		update_bounds();	// 無限ループしないように、align_items() は呼ばない。
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
		std::sort(mItems.begin(), mItems.end(), [](auto a, auto b) -> bool
		{
			return	a->priority() < b->priority();
		});

		on_reposition(items(), this->size());
		size_type	bounds	= {0, 0, 0};
		for (auto& item : mItems)
		{
			auto		tl	= item->position();
			auto		br	= tl + vec3<float>(item->size());
			if (bounds.width < br.x)
			{
				bounds.width	=  br.x;
			}
			if (bounds.height < br.y)
			{
				bounds.height	=  br.y;
			}
		}
		mItemsBounds	= bounds;
	}

	std::vector<item_type*>		mItems;
	bool						mAutoResize			= false;
	size_type					mItemsBounds		= {0, 0, 0};
};


AMTRS_NAMESPACE_END
#endif
