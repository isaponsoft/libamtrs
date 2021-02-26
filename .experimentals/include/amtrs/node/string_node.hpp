/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__string_node__hpp
#define	__libamtrs__node__string_node__hpp
#include "sprite_node.hpp"
#include "stringable_node.hpp"
AMTRS_NAMESPACE_BEGIN

// ============================================================================
//! 簡単なテキスト表示を行います。
// ----------------------------------------------------------------------------
//! ノードのサイズは常にテキストを描画するサイズと一致します。
// ----------------------------------------------------------------------------
template<class SuperT>
class	string_node
		: public stringable_node<sprite_node<SuperT>>
{
public:
	using	super_type		= stringable_node<sprite_node<SuperT>>;

	using	super_type::super_type;



protected:
	virtual void on_stringable_state() override
	{
		super_type::on_stringable_state();

		ref<g3d::texture>	tex;
		if (!super_type::get_font().empty() && !super_type::get_string().empty())
		{
			bitmap<rgba<uint8_t>>	bmp	= super_type::get_font()->create_bitmap(super_type::get_string());
			tex	= g3d::texture::create(std::move(bmp));
		}
		super_type::set_texture(tex);
	}
};


AMTRS_NAMESPACE_END
#endif
