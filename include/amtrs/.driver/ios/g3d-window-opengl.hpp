/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__window_ios_opengl__hpp
#define	__libamtrs__g3d__window_ios_opengl__hpp
// Framework
//		OpenGLES
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#include "../opengl/g3d-device.hpp"
AMTRS_G3D_NAMESPACE_BEGIN

class	window_ios_opengl
		: public window_ios<window>
{
	class	ios_opengl_device
			: public g3d::opengl::device
	{
		using	_base_type	= g3d::opengl::device;
	public:
		ios_opengl_device(window_ios_opengl* _window)
			: mWindow(_window)
		{}

		//! コンテキストが有効か調べます。
		virtual bool is_context_enable() const override
		{
			return	mContext ? true : false;
		}
/*
		virtual void release() override
		{
			if (mContext)
			{
				AMTRS_DEBUG_LOG("opengl device release");
				glDeleteFramebuffers(1, &mViewFramebuffer);
				glDeleteRenderbuffers(1, &mViewRenderbuffer);
				mContext	= nullptr;
			}
			_base_type::release();
		}
*/
		//! コンテキストをアクティブ化します。
		virtual void active() override
		{
			if (mContext)
			{
				if (![EAGLContext setCurrentContext:mContext])
				{
					auto err = glGetError();
					throw	std::runtime_error(amtrs::format<std::string>("EAGLContext setCurrentContext() : %s", opengl::get_error_string(err)));
				}
			}
		}

		//! コンテキストを非アクティブ化します。
		virtual void deactive() override
		{
			[EAGLContext setCurrentContext:nullptr];
		}


		//! コンテキストの大きさを取得します。
		virtual size_type size() const override
		{
			return	mWindow->size();
		}

		void initialize()
		{
			if (mContext)
			{
				return;
			}

			AMTRS_DEBUG_LOG("opengl device initialize");

			auto*			win         = mWindow;
			CAEAGLLayer*	eaglLayer   = (CAEAGLLayer*)[static_cast<window_ios_opengl*>(win)->view() layer];
			[eaglLayer setDrawableProperties:@{
				kEAGLDrawablePropertyRetainedBacking:	[NSNumber numberWithBool:FALSE],
				kEAGLDrawablePropertyColorFormat:		kEAGLColorFormatRGBA8
			}];

			mContext	= [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
			if (mContext == nil)
			{
				mContext	= [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
			}
			active();

			glGenFramebuffers(1, &mViewFramebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, mViewFramebuffer);

			glGenRenderbuffers(1, &mViewRenderbuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, mViewRenderbuffer);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mViewRenderbuffer);


			auto	ws	= size2<int>(size());
			glGenRenderbuffers(1, &mViewDepthbuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, mViewDepthbuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, ws.width, ws.height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mViewDepthbuffer);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mViewDepthbuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, mViewRenderbuffer);        
			if (![mContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer])
			{
			}

			
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
			}


			size2<GLint>	rbSize;
			glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &rbSize.width);
			glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &rbSize.height);
			AMTRS_DEBUG_LOG("surface size: %dx%d, windows size: %dx%d", (int)rbSize.width, (int)rbSize.height, (int)size().width, (int)size().height);

			// レンダリングの基本設定
			glDisable(GL_CULL_FACE);
	//		glEnable(GL_DEPTH_TEST);
	//		glDepthFunc(GL_LEQUAL);

			// テクスチャ系のパラメータのデフォルト値を設定
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//		glEnable(GL_BLEND);
	//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//		initialize_capacity();
		}

		virtual void present() override
		{
			if (mContext)
			{
				glFinish();
				if (![mContext presentRenderbuffer:GL_RENDERBUFFER])
				{
				}
			}
			_base_type::present();
		}


		EAGLContext*			mContext			= nullptr;
		window_ios_opengl*		mWindow				= nullptr;
		GLuint					mViewRenderbuffer;
		GLuint					mViewFramebuffer;
		GLuint					mViewDepthbuffer;
	};

	virtual g3d::device* get_g3d_device() override
	{
		if (mG3DDevice.empty())
		{
			mG3DDevice					= new ios_opengl_device(this);
			mG3DDevice->initialize();
		}
		return	mG3DDevice.get();
	}


	virtual bool repaint(std::function<void()> _repevent) override
	{
		if (_repevent)
		{
			_repevent();
		}
		else
		{
			repaint_event	ev({{0,0}, this->size()});
			fire_event(event_type::repaint, &ev);
		}
		return	true;
	}

	ref<ios_opengl_device>			mG3DDevice;
};


inline ref<window> window::create(size_type _size, create_params const& _cp)
{
	ref<window_ios_opengl>	thiz	= new window_ios_opengl();
	thiz->initialize(_size);
	return	thiz;
}


AMTRS_G3D_NAMESPACE_END
#endif
