/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__window_win32_opengl__hpp
#define	__libamtrs__g3d__window_win32_opengl__hpp
#include "../../logging.hpp"
#include "../../os/win32/def.hpp"
#include "../../os/win32/error_code.hpp"
#include "device-win32-opengl.hpp"
#include <windowsx.h>
AMTRS_G3D_NAMESPACE_BEGIN

class	window_win32_opengl
		: public window_win32<window>
{
public:
	class	device
			: public win32::opengl::device
	{
	public:
		using	_base_type	= win32::opengl::device;
		using	window_type	= window_win32_opengl;

		device(window_type* _window)
			: mWindow(_window)
		{}

		//! コンテキストの大きさを取得します。
		virtual size_type size() const override
		{
			return	mWindow->size();
		}

		virtual void present() override
		{
			glFlush();
			SwapBuffers(wglGetCurrentDC());
			_base_type::present();
		}


		window_type*	mWindow;
	};

	virtual g3d::device* get_g3d_device() override
	{
		if (mG3DDevice.empty())
		{
			mG3DDevice		= new device(this);
			mG3DDevice->initialize(hdc());
		}
		return	mG3DDevice.get();
	}


	virtual bool repaint() override
	{
		repaint_event	ev({{0,0}, this->size()});
		fire_event(event_type::repaint, &ev);
		return	true;
	}

	ref<device>	mG3DDevice;
};


inline ref<window> window::create(size_type _size, const create_params* _cp)
{
	ref<window_win32_opengl>	thiz	= new window_win32_opengl();
	thiz->initialize(_size, "AMTRS_WIN32_BASIC_WINDOW", "HELLO");
	return	thiz;
}


AMTRS_G3D_NAMESPACE_END
#endif
