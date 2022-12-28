/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__scissor__hpp
#define	__libamtrs__graphics__g3d__scissor__hpp
#include "type.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


struct	scissor
		: rect<int>
{
	using	rect::rect;
};


AMTRS_G3D_NAMESPACE_END
#endif
