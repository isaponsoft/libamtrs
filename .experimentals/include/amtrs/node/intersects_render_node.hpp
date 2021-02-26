/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__intersects_render_node__hpp
#define	__libamtrs__node__intersects_render_node__hpp
#include "intersects_notification_node.hpp"
AMTRS_NAMESPACE_BEGIN

// ****************************************************************************
//! 親ノードのエリア内から外れるとレンダリングを行わないようになります。
// ----------------------------------------------------------------------------
template<class BaseT>
class	intersects_render_node
		: public intersects_notification_node<BaseT>
{
public:
	using	super_type	= intersects_notification_node<BaseT>;

protected:
	virtual void on_enter() override
	{
		super_type::on_enter();
		this->set_visibility(this->is_intersects());
	}

	// ==============================================================
	//! 表示／非表示が切り替わったときに呼ばれます。
	// --------------------------------------------------------------
	virtual void on_intersects(bool _contains) override
	{
		super_type::on_intersects(_contains);
		this->set_visibility(_contains);
	}
};

AMTRS_NAMESPACE_END
#endif
