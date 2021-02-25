/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__xcb__g3d_device_opengl__hpp
#define	__libamtrs__driver__xcb__g3d_device_opengl__hpp
#include <chrono>
#include <thread>
#include <xcb/xcb.h>
#include <GL/glx.h> 
#include <GL/gl.h> 
#include <GL/glu.h>
#include <GLES2/gl2.h> 
#include "../opengl/g3d-device.hpp" 
AMTRS_G3D_NAMESPACE_BEGIN


class	device_xcb_opengl
		: public g3d::opengl::device
{
public:
	using	super_type	= g3d::opengl::device;

	device_xcb_opengl(Display* _display)
		: mDisplay(_display)
	{}

	//! コンテキストが有効か調べます。
	virtual bool is_context_enable() const override
	{
		return	false;
	}


	virtual void active() override
	{
	}

	virtual void deactive() override
	{
	}


	//! コンテキストの大きさを取得します。
	virtual size_type size() const override
	{
		return	size_type(0, 0);
	}

	void initialize()
	{
	}

	virtual void present() override
	{
		super_type::present();
	}


	Display*	mDisplay	= nullptr;
};




AMTRS_G3D_NAMESPACE_END
#endif
