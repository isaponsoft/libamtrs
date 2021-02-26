/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__x11__g3d_window_opengl_xcb__hpp
#define	__libamtrs__driver__x11__g3d_window_opengl_xcb__hpp
#include <chrono>
#include <thread>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
AMTRS_G3D_NAMESPACE_BEGIN


class	window_xcb_opengl
		: public window_xcb<window>
{
public:
	using	super_type			= window_xcb<window>;

	window_xcb_opengl()
	{
	}

	virtual g3d::device* get_g3d_device() override
	{
		return	nullptr;
	}
};



AMTRS_G3D_NAMESPACE_END
#endif
