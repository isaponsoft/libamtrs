/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__window_android_opengl__hpp
#define	__libamtrs__g3d__window_android_opengl__hpp
#include <chrono>
#include <thread>
#include "../../java/jp/libamtrs/window/GLSurfaceWindow.hpp"
#include "../../java/javax/microedition/khronos/egl/EGLConfig.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


class	window_android_opengl
		: public window_android<window>
{
	using	_base_type			= window_android<window>;
public:
	using	EGLConfig			= java::classes::javax::microedition::khronos::egl::EGLConfig;
	using	GLSurfaceWindow		= java::classes::jp::libamtrs::window::GLSurfaceWindow;
	using	opengl_device		= android::opengl::device;


	virtual void* get_context() const noexcept override
	{
		return	mGLSurface.get();
	}

	virtual bool repaint() override
	{
		if (!mSurfaceCreated || mDrawing || mG3DDevice.empty())
		{
//			AMTRS_DEBUG_LOG("None requestRender ============================================");
			return	false;
		}
		if (!mG3DDevice->is_context_enable())
		{
			return	false;
		}

//		AMTRS_DEBUG_LOG("requestRender ============================================");
		mDrawing	= true;
		java::jobj<GLSurfaceWindow>(mGLSurface.get()).requestRender();
		return	true;
	}

	virtual g3d::device* get_g3d_device() override
	{
		if (mG3DDevice.empty())
		{
			mG3DDevice					= new opengl_device();
			mG3DDevice->mSurfaceSize	= this->size();
		}
		return	mG3DDevice.get();
	}


protected:
	virtual void on_initialize() override
	{
		AMTRS_DEBUG_LOG("onInitialize (mSurfaceCreated = %s)", (mSurfaceCreated ? "True" : "false"));

		AMTRS_JAVA_JNIMETHOD_REGISTER(GLSurfaceWindow, window_android_opengl,
			AMTRS_JAVA_MAKE_JNIMETHOD(onDrawFrame),
			AMTRS_JAVA_MAKE_JNIMETHOD(onSurfaceChanged),
			AMTRS_JAVA_MAKE_JNIMETHOD(onSurfaceCreated),
			AMTRS_JAVA_MAKE_JNIMETHOD(onTouchEvent),
			AMTRS_JAVA_MAKE_JNIMETHOD(dispatchKeyEvent)
		);
 
		auto	activity		= os::android::get_activity();
		auto	clsGLSurface	= GLSurfaceWindow::find();
		auto	glSurface		= clsGLSurface.init(activity, static_cast<jlong>(reinterpret_cast<std::intptr_t>(this)));
		if (!glSurface.get())
		{
			throw	std::runtime_error("jp::amtrs::GLSurfaceWindow initialize error.");
		}

		activity.pushView(java::jobj<java::classes::android::view::View>(glSurface));
		mGLSurface	= glSurface;
 
		glSurface.onResume();
		AMTRS_TRACE_LOG("G3D(OpenGL) Initialized.");
	}



protected:
	virtual void on_lifecycle(lifecycle _lc) override
	{
		if (_lc == lifecycle::stop)
		{
			AMTRS_DEBUG_LOG("onLost");
			if (mSurfaceCreated)
			{
				mSurfaceCreated	= false;

				// onDrawFrame() が呼ばれている可能性があるので少し待つ
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				for (int i = 0; i < 3 && mDrawing; ++i)
				{
					AMTRS_DEBUG_LOG("Waiting finish with onDrawFrame().");
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
				mG3DDevice->on_lost();
			}
			java::jobj<GLSurfaceWindow>(mGLSurface.get()).onPause();
		}
		if (_lc == lifecycle::pause)
		{
			AMTRS_DEBUG_LOG("onPause");
		//	java::jobj<GLSurfaceWindow>(mGLSurface.get()).onPause();	// ロック画面で描画されなくなるので呼ばない。アプリ側で処理する。
		}
		if (_lc == lifecycle::start)
		{
			AMTRS_DEBUG_LOG("onResume");
			java::jobj<GLSurfaceWindow>(mGLSurface.get()).onResume();
		}
		_base_type::on_lifecycle(_lc);
	}

	void onDrawFrame()
	{
	//	AMTRS_DEBUG_LOG("onDrawFrame mSurfaceCreated = %d ============================================", (int)mSurfaceCreated);
		if (mSurfaceCreated)
		{
			repaint_event	ev({{0,0}, this->size()});
			fire_event(event_type::repaint, &ev);
		}
		mDrawing	= false;
	}

	void onSurfaceChanged(jint _width, jint _height)
	{
		AMTRS_DEBUG_LOG("onSurfaceChanged(%dx%d)", (int)_width, (int)_height);
		if (mG3DDevice)
		{
			mG3DDevice->mSurfaceSize	= size_type(_width, _height);
		}

		size_event	ev(size_type(_width, _height));
		fire_event(event_type::size_changed, &ev);

	}

	void onSurfaceCreated(java::jobj<EGLConfig> config)
	{
		AMTRS_DEBUG_LOG("onSurfaceCreated");
		if (mG3DDevice)
		{
			mG3DDevice->on_gain();
		}
		mSurfaceCreated	= true;
	}

	bool													mSurfaceCreated	= false;
	volatile bool											mDrawing		= false;
	java::globalref<jobject>								mGLSurface;
	ref<opengl_device>										mG3DDevice;
};


inline ref<window> window::create(size_type _size, const create_params* _cp)
{
	ref<window_android_opengl>	thiz	= new window_android_opengl();
	thiz->initialize();
	return	thiz;
}





AMTRS_G3D_NAMESPACE_END
#endif
