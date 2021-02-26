/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__hpp
#define	__libamtrs__g3d__hpp
#include "amtrs.hpp"
#include "geometry.hpp"
#include "graphics.hpp"

#define	AMTRS_G3D_NAMESPACE_BEGIN		AMTRS_NAMESPACE_BEGIN namespace g3d {
#define	AMTRS_G3D_NAMESPACE_END			} AMTRS_NAMESPACE_END
#define	AMTRS_G3DEX_NAMESPACE_BEGIN		AMTRS_G3D_NAMESPACE_BEGIN
#define	AMTRS_G3DEX_NAMESPACE_END		AMTRS_G3D_NAMESPACE_END

// keep order.
#include ".inc/g3d-attribute.hpp"
#include ".inc/g3d-device.hpp"				// depature
#include ".inc/g3d-basicshader.hpp"

#endif
