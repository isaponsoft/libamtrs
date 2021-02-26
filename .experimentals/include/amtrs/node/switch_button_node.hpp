/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__switch_button_node__hpp
#define	__libamtrs__node__switch_button_node__hpp
#include "button_node.hpp"
#include "switch_node.hpp"
AMTRS_NAMESPACE_BEGIN



// ****************************************************************************
//! スイッチを描画します。
// ----------------------------------------------------------------------------
//! 描画のみを行うため、タッチやクリックによる操作が必要な場合は button_node 
//! などと組み合わせて使用してください。
// ----------------------------------------------------------------------------
template<class SuperT, class MarkerT = circle_node<typename SuperT::node_type>>
class	switch_button_node
		: public button_node<switch_node<SuperT>>
{
public:
	using	super_type		= button_node<switch_node<SuperT>>;
	using	shader_type		= opengles_fill_shader;
	using	vertex			= typename shader_type::vertex;
	using	uniform			= typename shader_type::uniform;
	using	marker_type		= MarkerT;

	using	super_type::super_type;

protected:
	virtual void on_button_trigger(vec3<float> _pos) override
	{
		super_type::on_button_trigger(_pos);
		set_on_off(!get_on_off());
	}
};

AMTRS_NAMESPACE_END
#endif
