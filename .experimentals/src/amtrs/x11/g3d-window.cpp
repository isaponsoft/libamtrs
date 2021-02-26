/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/g3d/window.hpp>
AMTRS_G3D_NAMESPACE_BEGIN

ref<window> __x11xcb__create_vulkan_window(window::size_type _size, create_params const& _cp);


ref<window> window::create(size_type _size, create_params const& _cp)
{
	ref<window>	retval;
	if (_cp.type == api_type::api_vulkan)
	{
		retval	= __x11xcb__create_vulkan_window(_size, _cp);
	}
	else
	{
//		ref<window_xcb_opengl>	thiz	= new window_xcb_opengl();
//		thiz->initialize(_size);
//		retval	= thiz;
	}
	return	retval;
}


AMTRS_G3D_NAMESPACE_END
