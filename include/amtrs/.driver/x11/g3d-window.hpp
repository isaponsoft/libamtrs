/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__platform__x11__g3d_window__hpp
#define	__libamtrs__platform__x11__g3d_window__hpp
#include "g3d-window-opengl-xcb.hpp"
#include "g3d-window-vulkan-xcb.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


inline ref<window> window::create(size_type _size, create_params const& _cp)
{
	ref<window>	retval;
	if (_cp.type == api_type::api_vulkan)
	{
		ref<window_xcb_vulkan>	thiz	= new window_xcb_vulkan();
		thiz->initialize(_size);
		retval	= thiz;
	}
	else
	{
		ref<window_xcb_opengl>	thiz	= new window_xcb_opengl();
		thiz->initialize(_size);
		retval	= thiz;
	}
	return	retval;
}


AMTRS_G3D_NAMESPACE_END
#endif
