/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__stackpaging_node__hpp
#define	__libamtrs__node__stackpaging_node__hpp
#include "paging_node.hpp"
AMTRS_NAMESPACE_BEGIN


template<class SuperT, class Traits = paging_node_traits<SuperT>>
class	stackpaging_node
		: public paging_node<SuperT, Traits>
{
public:
	using	super_type		= paging_node<SuperT, Traits>;
	using	page_index_type	= typename super_type::page_index_type;
	using	node_type		= typename super_type::node_type;

	using	super_type::super_type;

	// ==============================================================
	//! スタックされているページがあるか調べる。
	// --------------------------------------------------------------
	//! スタックされている状態とは、カレントではないページを示します。
	// --------------------------------------------------------------
	bool stacking() const noexcept { return mStack.size() > 1; }


	// ==============================================================
	//! ページをポップする。
	// --------------------------------------------------------------
	void pop() { pop([](stackpaging_node*){}); }

	// ==============================================================
	//! ページをポップする。
	// --------------------------------------------------------------
	template<class Callback> void pop(Callback _callback)
	{
		// ポップするものが無いなら何もしない。
		if (mStack.empty())
		{
			return;
		}

		// 一つしかないならページを削除して終了
		if (mStack.size() == 1)
		{
			this->remove_child(mStack.back());
			_callback(this);
			return;
		}

		// ひとつ前のページの位置を調べる
		auto	lst	= mStack.back();
		mStack.pop_back();
		auto	pi	= super_type::find_page_index(mStack.back()).first;
		this->set_page_changed_listenr([=](page_index_type _pi)
		{
			this->set_page_changed_listenr({});
			this->remove_child(lst);
			_callback(this);
		});
		this->set_active_page(pi);
	}

	// ==============================================================
	//! ページを追加する。
	// --------------------------------------------------------------
	//! 追加されたノードがカレントページになる。
	// --------------------------------------------------------------
	node_type* push(node_type* _child)
	{
		auto	pi	= this->find_page_space(mVector);
		super_type::add_page(_child, pi);
		super_type::set_active_page(_child);
		return	_child;
	}

#if	0
	// ==============================================================
	//! ページを追加する。
	// --------------------------------------------------------------
	//! 追加されたノードがカレントページになる。
	// --------------------------------------------------------------
	node_type* add_child(node_type* _child)
	{
		auto	pi	= this->find_page_space(mVector);
		super_type::add_page(_child, pi);
		super_type::set_active_page(_child);
		return	_child;
	}
#endif

	// ==============================================================
	//! ページを追加する。
	// --------------------------------------------------------------
	//! 追加されたノードがカレントページになる。
	// --------------------------------------------------------------
	virtual auto insert_child(node_type* _pos, ref<node_type> _child) -> node_type* override
	{
		mStack.push_back(_child);
		super_type::insert_child(_pos, _child);
		super_type::set_active_page(_child);
		return	_child;
	}


	// ==============================================================
	//! ページを削除する。
	// --------------------------------------------------------------
	virtual bool remove_child(node_type* _child) override
	{
		// popせずに remove_from_parent() された時の保険
		auto	it	= std::find(mStack.begin(), mStack.end(), _child);
		if (it != mStack.end())
		{
			bool	change	= super_type::get_active_page() == *it;
			mStack.erase(it);
			if (change)
			{
				super_type::set_active_page(super_type::find_page_index(mStack.back()).first);
			}
		}
		return	super_type::remove_child(_child);
	}


	// ==============================================================
	//! ページを追加する方向を設定します。
	// --------------------------------------------------------------
	//! すでに追加済みのページには影響を与えません。
	// --------------------------------------------------------------
	void set_page_vector(page_index_type _vec)
	{
		mVector	= _vec;
	}

	node_type* front() const noexcept
	{
		return	mStack.empty() ? nullptr : mStack.back();
	}

private:
	page_index_type				mVector	= {1, 0, 0};
	std::vector<node_type*>		mStack;
};


AMTRS_NAMESPACE_END
#endif
