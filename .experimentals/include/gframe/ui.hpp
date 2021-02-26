/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libgframe__ui__hpp
#define	__libgframe__ui__hpp
#include <string>
#include <amtrs/node/@>
#include <amtrs/node/button_node.hpp>
#include "module.hpp"
GFRAME_NAMESPACE_BEGIN namespace ui {

float get_font_size();
void set_font_size(float _size);


class	button
		: public amtrs::button_node<amtrs::sprite_node<node>>
{
public:
	using	super_type	= button_node<sprite_node<node>>;
	using	super_type::super_type;

	static ref<button> create(vec3<float> _position, std::string const& _title, std::string const& _image, amtrs::rgba<float> _color);
};


} GFRAME_NAMESPACE_END
#endif
