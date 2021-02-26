/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__pagable_node__hpp
#define	__libamtrs__node__pagable_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN

template<class NodeT>
class	pager_node_if;


template<class NodeT>
class	page_node_if
{
public:
	enum class	page_event_type
	{
		visible,			//!< 一部もしくは全てが見えるようになった
		invisible,			//!< 完全に見えなくなった
		activated,			//!< 完全にアクティブ化された
		deactivated,		//!< 非アクティブ化された
	};

	using	page_type		= page_node_if<typename NodeT::node_type>;
	using	pager_type		= pager_node_if<typename NodeT::node_type>;


	bool page_visible() const noexcept { return mLastVisible == page_event_type::visible; }

	pager_type* get_pager() const noexcept { return mPagerIf; }

protected:
	virtual void on_page(page_event_type _page) = 0;

private:
	void fireVisible(page_event_type _event)
	{
		if (_event != mLastVisible)
		{
			mLastVisible	= _event;
			on_page(_event);
		}
	}

	pager_type*		mPagerIf		= nullptr;
	page_event_type	mLastVisible	= page_event_type::invisible;

	template<class>
	friend	class	pager_node;
};


template<class NodeT>
class	pagable_node
		: public NodeT
		, public page_node_if<typename NodeT::node_type>
{
	using	_base_type		= NodeT;
public:
	using	page_type		= page_node_if<typename NodeT::node_type>;
	using	pager_type		= pager_node_if<typename NodeT::node_type>;
	using	page_event_type	= typename page_type::page_event_type;

	using	_base_type::_base_type;


	virtual bool visibility() const noexcept override
	{
		return	_base_type::visibility() && page_type::page_visible();
	}

protected:
	virtual void on_page(page_event_type _page) override
	{
		if ((_page == page_event_type::invisible) && (_base_type::visibility()))
		{
			this->on_visibility(false);
		}
		if ((_page == page_event_type::visible) && (_base_type::visibility()))
		{
			this->on_visibility(true);
		}
	}
};


AMTRS_NAMESPACE_END
#endif
