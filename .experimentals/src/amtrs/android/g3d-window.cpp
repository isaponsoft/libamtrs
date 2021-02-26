/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <chrono>
#include <thread>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <amtrs/g3d.hpp>
AMTRS_G3D_NAMESPACE_BEGIN

ref<window> __android__create_opengl_window(window::size_type _size, create_params const& _cp);
ref<window> __android__create_vulkan_window(window::size_type _size, create_params const& _cp);


ref<window> window::create(size_type _size, create_params const& _cp)
{
	ref<window>	r;
	if (_cp.type == api_type::api_vulkan || _cp.type == api_type::api_auto)
	{
		r	= __android__create_vulkan_window(_size, _cp);
	}
	if (r.empty())
	{
		r	= __android__create_opengl_window(_size, _cp);
	}
	return	r;
}


AMTRS_G3D_NAMESPACE_END
