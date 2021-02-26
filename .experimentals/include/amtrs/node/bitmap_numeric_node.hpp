/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__bitmap_numeric_node__hpp
#define	__libamtrs__node__bitmap_numeric_node__hpp
#include "bitmap_font_node.hpp"
AMTRS_NAMESPACE_BEGIN
#if	0
template<class NodeT>
class	bitmap_numeric_node
		: public bitmap_font_node<NodeT>
{
	using	base_type	= bitmap_font_node<NodeT>;
	using	map_type	= std::map<int, int>;
	using	value_type	= typename map_type::value_type;
public:
	bitmap_numeric_node(size2<unsigned int> _divCount, g3d::texture* _texture)
		: base_type(_divCount, _texture)
	{}


	virtual void on_created() override
	{
		this->set_charmap({
			{'0',  0},
			{'1',  1},
			{'2',  2},
			{'3',  3},
			{'4',  4},
			{'5',  5},
			{'6',  6},
			{'7',  7},
			{'8',  8},
			{'8',  9},
			{'/', 10},
			{':', 11},
			{'.', 12},
			{'%', 13},
		});
	}


	void set_numner(int _num)
	{
		char	buff[100];
		snprintf(buff, sizeof(buff), "%d", _num);
		this->set_string(buff);
	}
};
#endif

AMTRS_NAMESPACE_END
#endif
