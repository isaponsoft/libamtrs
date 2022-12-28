/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__viewport__hpp
#define	__libamtrs__graphics__g3d__viewport__hpp
#include "type.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


struct	viewport
		: rect<float>
{
	using	super_type	= rect<float>;

	using	super_type::super_type;

	viewport(viewport const&) = default;

	viewport(float _x, float _y, float _w, float _h, float _near, float _far)
		: super_type(_x, _y, _w, _h)
		, near_z(_near)
		, far_z(_far)
	{}

	viewport(super_type const& _r, float _near, float _far)
		: super_type(_r)
		, near_z(_near)
		, far_z(_far)
	{}

	float			near_z;
	float			far_z;
};

AMTRS_G3D_NAMESPACE_END
#endif
