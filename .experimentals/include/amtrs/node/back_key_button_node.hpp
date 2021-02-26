/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__back_key_button_node__hpp
#define	__libamtrs__node__back_key_button_node__hpp
#include "node.hpp"
#include "back_key_node.hpp"
#include "button_node.hpp"
AMTRS_NAMESPACE_BEGIN

// ****************************************************************************
//! Androidのバックキー、もしくはそれに相当するキーイベントがあったときにボタンを押したものとして処理します。
// ----------------------------------------------------------------------------
//! バックキーが押されたときに、コールバックを呼び出します。コールバックが
//! true を返した場合、バックキーをアプリが処理したものとしてOSに通知します。
//! それ以外は button_node と同じです。
// ****************************************************************************
template<class NodeT>
class	back_key_button_node
		: public back_key_node<button_node<NodeT>>
{
	using	_base_type	= back_key_node<button_node<NodeT>>;
public:
	using	_base_type::_base_type;

protected:
	virtual bool on_back() override
	{
		_base_type::on_back();
		_base_type::on_button_trigger({0,0,0});
		return	true;
	}

private:
	std::function<bool()>	mOnBackCallback;
};



AMTRS_NAMESPACE_END
#endif
