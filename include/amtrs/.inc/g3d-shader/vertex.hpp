/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__g3dex__shader__vertex__hpp
#define __libamtrs__g3dex__shader__vertex__hpp
#include "../attribute.hpp"
#include "def.hpp"
AMTRS_G3DEX_SHADER_NAMESPACE_BEGIN

struct	color_vertex
{
	using	color_type		= rgba<float>;
	vec4<float>				pos;
	color_type				col;

	static constexpr g3d::attribute		attribute_info[] = 
	{
		g3d::attribute::make<decltype(pos)>("vertex_pos"),
		g3d::attribute::make<decltype(col)>("vertex_color")
	};
};



struct	texture_vertex
{
	using	color_type		= rgba<float>;
	vec4<float>		pos;
	vec2<float>		uv;
	color_type				col;

	static constexpr g3d::attribute		attribute_info[] = 
	{
		g3d::attribute::make<decltype(pos)>("vertex_pos"),
		g3d::attribute::make<decltype(uv)>("vertex_uv"),
		g3d::attribute::make<decltype(col)>("vertex_color")
	};
};


AMTRS_G3DEX_SHADER_NAMESPACE_END
#endif
