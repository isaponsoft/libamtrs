/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__g3dex__shader__uniform__hpp
#define __libamtrs__g3dex__shader__uniform__hpp
#include "../../graphics/@"
#include "../attribute.hpp"
#include "def.hpp"
AMTRS_G3DEX_SHADER_NAMESPACE_BEGIN

struct	simple_uniform
{
	mat44<float>		perspective;
	mat44<float>		coordinate;
	rgba<float>			color			= {1, 1, 1, 1};

	static constexpr g3d::attribute		uniform_info[] = 
	{
		g3d::attribute::make<decltype(perspective)>("perspective"),
		g3d::attribute::make<decltype(coordinate)>("coordinate"),
		g3d::attribute::make<decltype(color)>("color")
	};
};


struct	texture_uniform
{
	mat44<float>		perspective;
	mat44<float>		coordinate;
	ref<g3d::texture>	texture			= nullptr;
	rgba<float>			color			= {1, 1, 1, 1};

	static constexpr g3d::attribute		uniform_info[] = 
	{
		g3d::attribute::make<decltype(perspective)>("perspective"),
		g3d::attribute::make<decltype(coordinate)>("coordinate"),
		g3d::attribute::make<decltype(texture)>("texture1"),
		g3d::attribute::make<decltype(color)>("color"),
	};


	static_assert(sizeof(ref<g3d::texture>) == sizeof(void*));
};


AMTRS_G3DEX_SHADER_NAMESPACE_END
#endif
