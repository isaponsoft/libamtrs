/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__opengl__gl_types__hpp
#define	__libamtrs__graphics__opengl__gl_types__hpp
#include "g3d-opengl-def.hpp"
AMTRS_OPENGL_NAMESPACE_BEGIN



static constexpr GLenum get_blend_mode(g3d::draw_status::blend_type _type)
{
	using	type	= g3d::draw_status::blend_type;
	return	_type == type::zero				? GL_ZERO
		:	_type == type::one				? GL_ONE
		:	_type == type::src_color		? GL_SRC_COLOR
		:	_type == type::invrs_src_color	? GL_ONE_MINUS_SRC_COLOR
		:	_type == type::dest_color		? GL_DST_COLOR
		:	_type == type::invrs_dest_color	? GL_ONE_MINUS_DST_COLOR
		:	_type == type::src_alpha		? GL_SRC_ALPHA
		:	_type == type::invrs_src_alpha	? GL_ONE_MINUS_SRC_ALPHA
		:	_type == type::dest_alpha		? GL_DST_ALPHA
		:	_type == type::invrs_dest_alpha	? GL_ONE_MINUS_DST_ALPHA
		:	GL_ZERO;
}


static constexpr GLint get_texture2d_flter(g3d::draw_status::texture2d_filter_type _type)
{
	using	type	= g3d::draw_status::texture2d_filter_type;
	return	_type == type::point				? GL_NEAREST
		:	_type == type::liner				? GL_LINEAR
		:	_type == type::anisotropic			? GL_LINEAR
		:	GL_LINEAR;
}



AMTRS_OPENGL_NAMESPACE_END
#endif
