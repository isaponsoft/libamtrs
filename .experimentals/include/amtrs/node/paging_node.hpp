/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__paging_node__hpp
#define	__libamtrs__node__paging_node__hpp
#include "children_transform_node.hpp"
AMTRS_NAMESPACE_BEGIN


// ****************************************************************************
//! ３D空間におけるページ切り替えを処理を行います。
// ----------------------------------------------------------------------------
//! 自身のノードの大きさをページとして扱い、ページ単位で children_matrix を
//! 操作します。
// ----------------------------------------------------------------------------
template<class BaseT, class Traits = paging_node_traits<BaseT>>
class	paging_node
		: public children_transform_node<BaseT>
{
public:
	using	super_type				= children_transform_node<BaseT>;
	using	node_type				= typename super_type::node_type;
	using	paging_traits_type		= Traits;
	using	value_type				= typename super_type::value_type;
	using	position_type			= typename super_type::position_type;
	using	size_type				= typename super_type::size_type;
	using	matrix_type				= typename super_type::matrix_type;
	using	page_index_type			= vec3<int>;
	using	page_changing_listenr	= page_index_type(page_index_type);
	using	page_changed_listenr	= void(page_index_type);

	using	super_type::add_child;


	// ==============================================================
	//! 指定したページインデックスのノードを返す。
	// --------------------------------------------------------------
	//! 見つからない場合は nullptr を返す。同じページが複数ある場合は
	//! 最初に見つかったものを返す。
	// --------------------------------------------------------------
	node_type* get_page(page_index_type _page) const noexcept;


	// ==============================================================
	//! 現在アクティブなページを返す。。
	// --------------------------------------------------------------
	node_type* get_active_page() const noexcept { return get_page(active_page()); }


	// ==============================================================
	//! 指定したノードのページインデックスを返す。
	// --------------------------------------------------------------
	//! 見つからない場合は {{0,0,0}, false} を返す。
	// --------------------------------------------------------------
	std::pair<page_index_type, bool> find_page_index(node_type* _node) const noexcept;


	// ==============================================================
	//! ページ単位での位置を指定しながら子を追加する。
	// --------------------------------------------------------------
	//!
	// --------------------------------------------------------------
	void add_page(node_type* _node, page_index_type _page);


	// ==============================================================
	//! ページを追加します。
	// --------------------------------------------------------------
	//! すでにページが存在する場合は_pagevectorの方向にページを移動します。
	// --------------------------------------------------------------
	void push_page(node_type* _node, page_index_type _pagevector);


	// ==============================================================
	//! 指定したページに切り替える。
	// --------------------------------------------------------------
	//!
	// --------------------------------------------------------------
	void set_active_page(page_index_type _page, bool _animation = true);

	// ==============================================================
	//! 指定したページに切り替える。
	// --------------------------------------------------------------
	//!
	// --------------------------------------------------------------
	void set_active_page(node_type* _node, bool _animation = true);

	// ==============================================================
	//! アクティブなページインデックスを取得する。
	// --------------------------------------------------------------
	//!
	// --------------------------------------------------------------
	page_index_type active_page() const noexcept { return mCurrentPage; }


	// ========================================================================
	//! ページが変更されようとしたときに呼ばれるリスナ。
	// ------------------------------------------------------------------------
	//! 
	// ------------------------------------------------------------------------
	void set_page_changing_listenr(std::function<page_changing_listenr> _listener) { mChangingListener = std::move(_listener); }


	void set_page_changed_listenr(std::function<page_changed_listenr> _listener) { mChangedListener = std::move(_listener); }


	// ========================================================================
	//! ページの一覧を返す。
	// ------------------------------------------------------------------------
	auto pages() const noexcept { return mPages; }


	// ==============================================================
	//! 指定した位置から指定した方向に向かって空いているページを探す
	// --------------------------------------------------------------
	//!
	// --------------------------------------------------------------
	auto find_page_space(page_index_type _vec, page_index_type _start = {0,0,0}) ->page_index_type;

	size_t page_count() const noexcept { return mPages.size(); }


	page_index_type compute_page_index(position_type _position) const noexcept;
	position_type compute_page_position(page_index_type _page_index) const noexcept;

protected:
	virtual void on_size_change(size_type& _size) override
	{
		super_type::on_size_change(_size);
		reposition_pages();
	}

	virtual void on_child(node_type* _child, typename node_type::on_child_event _event) override
	{
		super_type::on_child(_child, _event);
		if (_event == node_type::on_child_event::remove)
		{
			auto it	= find_if(mPages.begin(), mPages.end(), [c = _child](auto& page)
					{
						return	page.node == c;
					});
			if (it != mPages.end())
			{
				mPages.erase(it);
			}
		}
	}


	void reposition_pages()
	{
		for (auto& data : mPages)
		{
			if (!data.node)
			{
				continue;
			}
			auto	pos	= position_type(data.page) * position_type(this->size());
			data.node->set_position(pos);
		}
		super_type::set_children_position(compute_children_translate(active_page()));
	}

	//! 指定するページを表示するためのトランスレート座標。
	position_type compute_children_translate(page_index_type _page) const noexcept
	{
		return	(-position_type(_page)) * position_type(this->size());
	}

private:
	struct	page_data
	{
		node_type*		node	= nullptr;
		page_index_type	page	= { 0, 0, 0 };
	};


	page_index_type							mCurrentPage	= page_index_type(0, 0, 0);
	std::function<page_changing_listenr>	mChangingListener;
	std::function<page_changed_listenr>		mChangedListener;
	std::list<page_data>					mPages;
	bool									mAutoReposition	= true;
	bool									mAutoResize		= true;
};


template<class BaseT, class Traits>
auto paging_node<BaseT, Traits>::get_page(page_index_type _page) const noexcept -> node_type*
{
	auto	it	= std::find_if(mPages.begin(), mPages.end(), [&](auto& data)
				{
					return	data.page == _page;
				});
	return	it != mPages.end() ? it->node : nullptr;
}


template<class BaseT, class Traits>
auto paging_node<BaseT, Traits>::find_page_index(node_type* _node) const noexcept -> std::pair<page_index_type, bool>
{
	for (auto& pg : mPages)
	{
		if (pg.node == _node)
		{
			return	{ pg.page, true };
		}
	}
	return	{ {0,0,0}, false };
}


template<class BaseT, class Traits>
void paging_node<BaseT, Traits>::add_page(node_type* _node, page_index_type _page)
{
	mPages.push_back({_node, _page});
	super_type::add_child(_node);
	reposition_pages();
}



template<class BaseT, class Traits>
void paging_node<BaseT, Traits>::push_page(node_type* _node, page_index_type _pagevector)
{
	add_page(_node, this->find_page_space(_pagevector));
}


template<class BaseT, class Traits>
void paging_node<BaseT, Traits>::set_active_page(page_index_type _page, bool _animation)
{
	if (mChangingListener)
	{
		_page	= mChangingListener(_page);
	}

	if (mCurrentPage == _page) { return; }
	if (!get_page(_page))
	{
		return;
	}


	mCurrentPage	= _page;

	auto	pos	= compute_children_translate(_page);
	if (_animation)
	{
		paging_traits_type{}.page_change_transform(super_type::children_transform(), pos, this, [this]()
		{
			if (mChangedListener)
			{
				mChangedListener(mCurrentPage);
			}
		});
	}
	else
	{
		super_type::set_children_position(pos);
		if (mChangedListener)
		{
			mChangedListener(mCurrentPage);
		}
	}
}


template<class BaseT, class Traits>
void paging_node<BaseT, Traits>::set_active_page(node_type* _node, bool _animation)
{
	auto	r	= find_page_index(_node);
	if (r.second)
	{
		set_active_page(r.first, _animation);
	}
}


template<class BaseT, class Traits>
auto paging_node<BaseT, Traits>::compute_page_index(position_type _position) const noexcept -> page_index_type
{
	page_index_type	retval(0, 0, 0);
	auto			sz(this->size());
	if (sz.width > 0)
	{
		retval.x	= (typename page_index_type::value_type)(_position.x / sz.width);
	}
	if (sz.height > 0)
	{
		retval.y	= (typename page_index_type::value_type)(_position.y / sz.height);
	}
	if (sz.depth > 0)
	{
		retval.z	= (typename page_index_type::value_type)(_position.z / sz.depth);
	}
	return	retval;
}


template<class BaseT, class Traits>
auto paging_node<BaseT, Traits>::compute_page_position(page_index_type _page_index) const noexcept -> position_type
{
	return	-compute_children_translate(_page_index);
}




template<class BaseT, class Traits>
auto paging_node<BaseT, Traits>::find_page_space(page_index_type _vec, page_index_type _start) -> page_index_type
{
	page_index_type	retval(_start);
	while (get_page(retval))
	{
		retval += _vec;
	}
	return	retval;
}



template<class SuperT, class... Args>
struct	paging_node_traits
{
	using	paging_action_type		= move_by_action<waveform::linear<float>>;

	template<class T, class O, class Callback>
	void page_change_transform(T* _transform, vec3<float> _pos, O* _owner, Callback&& _callback)
	{
		_owner->run_action(new paging_action_type(_transform, _pos, 0.12f))
		->set_finish_callback([listener = std::move(_callback)]()
		{
			listener();
		});
	}
};

AMTRS_NAMESPACE_END
#endif
