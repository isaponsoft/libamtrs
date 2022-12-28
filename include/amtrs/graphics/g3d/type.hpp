/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__type__hpp
#define	__libamtrs__graphics__g3d__type__hpp
#include "../../memory/view.hpp"
#include "../color.hpp"
#include "../geometory/vec.hpp"
#include "../geometory/size.hpp"
AMTRS_G3D_NAMESPACE_BEGIN

using	value_type	= float;
using	vec2_t		= graph::vec2<value_type>;
using	vec3_t		= graph::vec3<value_type>;
using	vec4_t		= graph::vec4<value_type>;
using	mat44_t		= graph::mat44<value_type>;
using	size2_t		= graph::size2<value_type>;
using	rgba8_t		= graph::rgba<uint8_t>;
using	rgb8_t		= graph::rgb<uint8_t>;
using	rgba_t		= graph::rgba<value_type>;
using	rgb_t		= graph::rgb<value_type>;

enum class	api_type
{
	api_opengl		= 0,
	api_vulkan		= 1,
	api_d3d12		= 2,
};


enum class	element_type
{
	float_value,
	texture2d_value,
};


enum class	format_types
{
	unknown_value	= -1,
	constants_value	= 0,
	texture_value	= 1,
};

enum class	primitive_type
{
	triangles,
	triangle_strip,
	triangle_fan,
};


enum class	shader_stage
{
	none_stage			= -1,		//!< not initialized
	vertex_stage		= 0,
	tesslation_stage,
	geometory_stage,
	fragment_stage,
	compute_stage,
	_max
};

class	cmdlist_pipeline;
class	commandexecuter;
class	commandlist;
class	constants;
class	framebuffer;
class	imagesampler;
class	shadermodule;
class	shaderset;
class	textureset;
class	uniformset;
class	vertexbuffer;

struct	scissor;
struct	viewport;

AMTRS_G3D_NAMESPACE_END
#endif
