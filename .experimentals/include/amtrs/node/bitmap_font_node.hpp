/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__bitmap_font_node__hpp
#define	__libamtrs__node__bitmap_font_node__hpp
#include <vector>
#include "node.hpp"
#include "batch_sprite_node.hpp"
#include "sprite_node.hpp"
AMTRS_NAMESPACE_BEGIN

#if	0
template<class NodeT>
class	bitmap_font_node
		: public batch_sprite_node<NodeT>
{
	using	base_type	= batch_sprite_node<NodeT>;
	using	map_type	= std::map<int, int>;
	using	value_type	= typename map_type::value_type;
public:
	bitmap_font_node(size2<unsigned int> _divCount, g3d::texture* _texture)
		: base_type(_divCount, _texture)
	{}


	void set_charmap(std::initializer_list<value_type> _chars)
	{
		mCharMap.clear();
		mCharMap	= map_type(_chars.begin(), _chars.end());
	}

	void set_string(std::string_view _string)
	{
		base_type::mChars.clear();
		auto	filter	= make_bucket(std::begin(_string), std::end(_string), [this](auto c) -> int
		{
			return	mCharMap[c];
		});
		base_type::mChars.assign(filter.begin(), filter.end());
		base_type::build_vertex();
	}


protected:
	map_type	mCharMap;
};
#endif
AMTRS_NAMESPACE_END
#endif
