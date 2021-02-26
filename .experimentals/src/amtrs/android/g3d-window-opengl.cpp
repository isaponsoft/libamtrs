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
#include <amtrs/.driver/g3d-opengl-device.hpp>
#include <amtrs/.driver/android-window-window.hpp>
#include <amtrs/java/jp/libamtrs/window/GLSurfaceWindow.hpp>
#include <amtrs/java/javax/microedition/khronos/egl/EGLConfig.hpp>
#define	AMTRS_ANDROID_OPENGL_NAMESPACE_BEGIN	AMTRS_G3D_NAMESPACE_BEGIN namespace android { namespace opengl {
#define	AMTRS_ANDROID_OPENGL_NAMESPACE_END	} } AMTRS_G3D_NAMESPACE_END
AMTRS_ANDROID_OPENGL_NAMESPACE_BEGIN

// ============================================================================
//! OpenGL のコンテキストを抽象化したWin32版の共通インターフェース。
// ----------------------------------------------------------------------------
class	device
		: public g3d::opengl::device
{
public:
	using	super_type	= g3d::opengl::device;

	device() = default;
	~device() = default;

	using	super_type::on_gain;
	using	super_type::on_lost;



	//! コンテキストをアクティブ化します。
	virtual void active() override
	{
	}

	//! コンテキストを非アクティブ化します。
	virtual void deactive() override
	{
	}

	//! コンテキストの大きさを取得します。
	virtual size_type size() const override
	{
		return	mSurfaceSize;
	}

	virtual void present() override
	{
		if (is_context_enable())
		{
			glFlush();
		}
		super_type::present();
	}

	size_type						mSurfaceSize	= {0, 0};

	friend	amtrs::g3d::device;
};
AMTRS_ANDROID_OPENGL_NAMESPACE_END
AMTRS_G3D_NAMESPACE_BEGIN


class	window_android_opengl
		: public window_android<window>
{
	using	_base_type			= window_android<window>;
public:
	using	EGLConfig			= java::classes::javax::microedition::khronos::egl::EGLConfig;
	using	GLSurfaceWindow		= java::classes::jp::libamtrs::window::GLSurfaceWindow;
	using	opengl_device		= android::opengl::device;

	virtual ~window_android_opengl()
	{
		AMTRS_SYSTRACE_LOG("Destroy opengl window");
	}


	virtual void* get_context() const noexcept override
	{
		return	mGLSurface.get();
	}

	virtual bool repaint(std::function<void()> _callback) override
	{
		if (!mG3DDevice->is_context_enable() || !mSurfaceCreated || mDrawing || mG3DDevice.empty())
		{
			return	false;
		}

		mDrawing		= true;
		mDrawCallback	= std::move(_callback);
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
		AMTRS_SYSTRACE_LOG("onInitialize (mSurfaceCreated = %s)", (mSurfaceCreated ? "True" : "false"));
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
			AMTRS_ERROR_LOG("GLSurfaceWindow initialize error");
			throw	std::runtime_error("jp::amtrs::GLSurfaceWindow initialize error.");
		}

		activity.pushView(java::jobj<java::classes::android::view::View>(glSurface));
		mGLSurface	= glSurface;
 
		glSurface.onResume();
		AMTRS_SYSTRACE_LOG("G3D(OpenGL) Initialized.");
	}



protected:
	virtual void on_lifecycle(lifecycle _lc) override
	{
		if (_lc == lifecycle::stop)
		{
			AMTRS_SYSTRACE_LOG("[%p] onLost", this);
			if (mSurfaceCreated)
			{
				mSurfaceCreated	= false;
				mG3DDevice->on_lost();
			}
			java::jobj<GLSurfaceWindow>(mGLSurface.get()).onPause();
		}
		if (_lc == lifecycle::pause)
		{
			AMTRS_SYSTRACE_LOG("[%p] onPause", this);

			// onDrawFrame() が呼ばれている可能性があるので少し待つ
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			for (int i = 0; i < 3 && mDrawing; ++i)
			{
				AMTRS_DEBUG_LOG("Waiting finish with onDrawFrame().");
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		//	java::jobj<GLSurfaceWindow>(mGLSurface.get()).onPause();	// ロック画面で描画されなくなるので呼ばない。アプリ側で処理する。
		}
		if (_lc == lifecycle::start)
		{
			AMTRS_SYSTRACE_LOG("[%p] onResume", this);
			java::jobj<GLSurfaceWindow>(mGLSurface.get()).onResume();
		}
		_base_type::on_lifecycle(_lc);
	}

	void onDrawFrame()
	{
		if (mSurfaceCreated)
		{
			if (mDrawCallback)
			{
				mDrawCallback();
				mDrawCallback	= {};
			}
			else
			{
				repaint_event	ev({{0,0}, this->size()});
				fire_event(event_type::repaint, &ev);
			}
			mDrawing	= false;
		}
	}

	void onSurfaceChanged(jint _width, jint _height)
	{
		AMTRS_SYSTRACE_LOG("[%p] onSurfaceChanged(%dx%d)", this, (int)_width, (int)_height);
		if (mG3DDevice)
		{
			mG3DDevice->mSurfaceSize	= size_type(_width, _height);
		}

		size_event	ev(size_type(_width, _height));
		fire_event(event_type::size_changed, &ev);

	}

	void onSurfaceCreated(java::jobj<EGLConfig> config)
	{
		AMTRS_SYSTRACE_LOG("[%p] onSurfaceCreated", this);
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
	std::function<void()>									mDrawCallback;
};


ref<window> __android__create_opengl_window(window::size_type _size, create_params const& _cp)
{
	AMTRS_INFO_LOG("Use opengl");
	ref<window_android_opengl>	thiz	= new window_android_opengl();
	thiz->initialize();
	return	thiz;
}



AMTRS_G3D_NAMESPACE_END
