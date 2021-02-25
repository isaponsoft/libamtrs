/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__basicshader__hpp
#define	__libamtrs__g3d__basicshader__hpp
#include <functional>
AMTRS_G3D_NAMESPACE_BEGIN namespace basicshader {

struct	worlduniform
{
	mat44<float>	camera;
	mat44<float>	world;
	rgba<float>		color;

	static constexpr g3d::attribute		attributes[] = 
	{
		g3d::attribute::make<decltype(camera)>("camera"),
		g3d::attribute::make<decltype(world)>("world"),
		g3d::attribute::make<decltype(color)>("color"),
	};
};

struct	fillshader
{
	using	uniform	= worlduniform;

	struct	vertex
	{
		vec4<float>	position;
		rgba<float>	color;

		static constexpr g3d::attribute		attribute_info[] = 
		{
			g3d::attribute::make<decltype(position)>("vertex_pos"),
			g3d::attribute::make<decltype(color)>("vertex_color")
		};
	};

	static material create(commandlist* _updatecmd, api_type _api);
};


struct	textureshader
{
	using	uniform	= worlduniform;

	struct	vertex
	{
		vec4<float>	position;
		rgba<float>	color;
		vec2<float>	uv;

		static constexpr g3d::attribute		attribute_info[] = 
		{
			g3d::attribute::make<decltype(position)>("vertex_pos"),
			g3d::attribute::make<decltype(color)>("vertex_color"),
			g3d::attribute::make<decltype(uv)>("vertex_uv"),
		};
	};

	static material create(commandlist* _updatecmd, api_type _api);
};

} AMTRS_G3D_NAMESPACE_END
#endif

