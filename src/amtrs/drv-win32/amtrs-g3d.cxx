/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include "amtrs-g3d.hpp"
AMTRS_G3D_NAMESPACE_BEGIN

ref<device> device::create(createparams& _cp)
{
	switch (_cp.api)
	{
		case api_type::api_d3d12 :	return	os::g3d_d3d12_create(_cp);
		case api_type::api_opengl :	return	os::g3d_opengl_create(_cp);
		case api_type::api_vulkan :	return	os::g3d_vulkan_create(_cp);
	}
	return	{};
}

AMTRS_G3D_NAMESPACE_END
