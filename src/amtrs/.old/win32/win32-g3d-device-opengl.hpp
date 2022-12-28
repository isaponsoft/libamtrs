/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__device__win32__opengl__hpp
#define	__libamtrs__g3d__device__win32__opengl__hpp
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")

#include ".api-windows.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>
#include "opengl.inc/glext-value.hpp"
#define	AMTRS_USE_DYNAMIC_OPENGL_INIT	1

#include "../opengl/g3d-device.hpp"
#define	AMTRS_WIN32_OPENGL_NAMESPACE_BEGIN	AMTRS_G3D_NAMESPACE_BEGIN namespace win32 { namespace opengl {
#define	AMTRS_WIN32_OPENGL_NAMESPACE_END	} } AMTRS_G3D_NAMESPACE_END
AMTRS_WIN32_OPENGL_NAMESPACE_BEGIN

using namespace os::win32;

// ============================================================================
//! OpenGL のコンテキストを抽象化したWin32版の共通インターフェース。
// ----------------------------------------------------------------------------
class	device
		: public g3d::opengl::device
{
public:
	virtual ~device()
	{
		wglDeleteContext(mGLRC);
	}

	//! コンテキストをアクティブ化します。
	virtual void active() override
	{
		wglMakeCurrent(mDC, mGLRC);
	}

	//! コンテキストを非アクティブ化します。
	virtual void deactive() override
	{
		wglMakeCurrent(mDC, nullptr);
	}

	//! コンテキストが有効か調べます。
	virtual bool is_context_enable() const override
	{
		return	mGLRC != nullptr;
	}

	// 
	void initialize(HDC _hdc)
	{
		// ピクセルフォーマットを設定
		PIXELFORMATDESCRIPTOR	pfd;
		std::memset(&pfd, 0, sizeof(pfd));
		pfd.nVersion		= 1;
		pfd.dwFlags			= PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;;
		pfd.iPixelType		= PFD_TYPE_RGBA;
		pfd.cColorBits		= 32;
		pfd.cDepthBits		= 24;
		pfd.cStencilBits	= 8;


		GLuint		uPixelFormat	= ChoosePixelFormat(_hdc, &pfd);	
		if (!uPixelFormat)
		{
			throw	std::system_error(make_last_error_code(), "ChoosePixelFormat()");
		}
		if (!SetPixelFormat(_hdc, uPixelFormat, &pfd))
		{
			throw	std::system_error(make_last_error_code(), "SetPixelFormat()");
		}

		mGLRC	= wglCreateContext(_hdc);
		if (!mGLRC)
		{
			throw	std::system_error(make_last_error_code(), "wglCreateContext()");
		}

		wglMakeCurrent(_hdc, mGLRC);
		mDC		= _hdc;

		auto	wglCreateContextAttribsARB	= (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
		if (wglCreateContextAttribsARB)
		{
			static const int  attr[]= {
				WGL_CONTEXT_MAJOR_VERSION_ARB,   2,
				WGL_CONTEXT_MINOR_VERSION_ARB,   0,
				WGL_CONTEXT_FLAGS_ARB,           0,
				WGL_CONTEXT_PROFILE_MASK_ARB,
				WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0,
			};
		
			// 新しい HGLRC の作成
			HGLRC	hglrc		= wglCreateContextAttribsARB(mDC, nullptr, attr);
			wglMakeCurrent(mDC, hglrc);
			wglDeleteContext(mGLRC);
			mGLRC	= hglrc;

			// 拡張APIを取得
			#if	AMTRS_USE_DYNAMIC_OPENGL_INIT
			#include "opengl.inc/glext-init.hpp"
			#endif
			mSupportExtAPI	= true;
		}
	}

protected:
	HDC			mDC				= nullptr;		//!< 派生先で設定
	HGLRC		mGLRC			= nullptr;		//!< 派生先で設定
	bool		mSupportExtAPI	= false;
};
AMTRS_WIN32_OPENGL_NAMESPACE_END
AMTRS_G3D_NAMESPACE_BEGIN
/*
inline ref<device> device::create(window* _window)
{
	class	device
			: public win32::opengl::device
	{
	public:
		using	base_type	= win32::opengl::device;
		using	window_type	= window_win32_opengl;

		//! コンテキストの大きさを取得します。
		virtual size_type size() const override
		{
			return	mWindow->size();
		}

		void initialize(window* _window)
		{
			mWindow	= static_cast<window_type*>(_window);
			base_type::initialize(mWindow->hdc());
		}

		virtual void present() override
		{
			glFlush();
			SwapBuffers(wglGetCurrentDC());
		}


		ref<window_type>	mWindow;
	};


	ref<device>	thiz	= new device();
	thiz->initialize(_window);
	return	thiz;
}
*/

template<class BitmapT>
inline ref<device> device::create(BitmapT* _bitmap)
{
	using namespace os::win32;

	class	device
			: public win32::opengl::device
	{
	public:
		using	_base_type	= win32::opengl::device;

		device(BitmapT* _bmp)
			: mBitmap(_bmp)
		{}

		~device()
		{
			if (mDC)
			{
				ReleaseDC(nullptr, mDC);
			}
		}


		//! コンテキストの大きさを取得します。
		virtual size_type size() const override
		{
			return	mBitmap->size();
		}

		virtual void present() override
		{
			glReadPixels(0, 0, size().width, size().height, GL_RGBA, GL_UNSIGNED_BYTE, mBitmap->pixels().data());
			_base_type::present();
		}


		void initialize()
		{
			// デスクトップのデバイスコンテキストを使う
			// (コンパチブルなどのメモリデバイスだと GDI Generic driver が使われて今うので注意！)
			mDC	= GetDC(nullptr);
			if (!mDC)
			{
				throw	std::system_error(make_last_error_code(), "GetDC()");
			}
			_base_type::initialize(mDC);
		}

		BitmapT*	mBitmap;
	};


	ref<device>	thiz	= new device(_bitmap);
	thiz->initialize();
	return	thiz;
}

AMTRS_G3D_NAMESPACE_END
#endif
