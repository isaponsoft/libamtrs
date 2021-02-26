/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__listener__remove_child__hpp
#define	__libamtrs__node__listener__remove_child__hpp
#include <functional>
#include "../node.hpp"
AMTRS_NAMESPACE_BEGIN

// ****************************************************************************
//! remove_child() をリスナー化します。
// ----------------------------------------------------------------------------
//! このノードを継承すると
// ----------------------------------------------------------------------------
template<class BaseT>
class	remove_child_node
		: public BaseT
{
public:
	using	super_type	= BaseT;
	using	node_type	= typename super_type::node_type;

	// ==============================================================
	//! リスナを登録します。
	// --------------------------------------------------------------
	//! リスナが true を返した場合はそれ以上なにもしません。
	//! false を返した場合は既定の処理を行います。
	// --------------------------------------------------------------
	void set_remove_child_listener(std::function<bool(node_type*)> _listener)
	{
		mListener	= std::move(_listener);
	}

	virtual bool remove_child(node_type* _node) override
	{
		bool	cancel	= false;
		if (mListener)
		{
			cancel	= mListener(_node);
		}
		if (cancel)
		{
			return	true;
		}
		return	super_type::remove_child(_node);
	}

private:
	std::function<bool(node_type*)>	mListener;
};

AMTRS_NAMESPACE_END
#endif
