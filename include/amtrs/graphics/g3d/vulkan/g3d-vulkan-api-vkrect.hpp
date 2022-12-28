/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkrect__hpp
#define	__libamtrs__g3d__vulkan__vkrect__hpp
#include "../../geometory/rect.hpp"
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN

template<class T>
struct	vkrect
		: graph::rect<T>
{
	using	super_type	= graph::rect<T>;

	using	super_type::super_type;

	vkrect(VkRect2D const& _s)
		: super_type(_s.offset, _s.extent)
	{}

	operator VkRect2D () const noexcept
	{
		VkRect2D	r;
		r.offset 	= { super_type::x,     super_type::y      },;
		r.extent	= { super_type::width, super_type::height };
		return	r;
	}
};

AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
