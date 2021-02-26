/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <thread>
#include <chrono>
#include <deque>

#include <android/native_window.h>
#include <android/native_window_jni.h>


#include <amtrs/.driver/android-api-amtrs_activity.hpp>
#include <amtrs/.driver/android-window-window.hpp>
#include <amtrs/java/android/graphics/PixelFormat.hpp>
#include <amtrs/java/android/view/Surface.hpp>
#include <amtrs/java/android/view/SurfaceHolder.hpp>
#include <amtrs/java/jp/libamtrs/window/SurfaceView.hpp>

#define	VK_NO_PROTOTYPES	1

#include <amtrs/g3d.hpp>
#include <amtrs/.driver/g3d-vulkan.hpp>


AMTRS_G3D_NAMESPACE_BEGIN


using namespace	vulkan;

namespace	jvc				= amtrs::java::classes;
using		PixelFormat		= jvc::android::graphics::PixelFormat;
using		SurfaceHolder	= jvc::android::view::SurfaceHolder;
using		SurfaceView		= jvc::jp::libamtrs::window::SurfaceView;

class	vulkan_window;

class	android_vkdevice
		: public vkdevice
{
public:

	//! vkdevice から呼ばれる。
	//! _initdataにプラットフォーム固有の初期化情報を格納して返す。
	virtual void on_initialize(initdata& _initdata) override
	{
		_initdata.instance	= vkref<VkInstance>::create("libamtrs", {"VK_KHR_surface", "VK_KHR_android_surface"},{"VK_LAYER_KHRONOS_validation"});
		_initdata.surface	= vkref<VkSurfaceKHR>::create(_initdata.instance, mWindow);
	}

	ANativeWindow*		mWindow		= nullptr;
};




// ============================================================================
//! VULKAN WINDOW
// ----------------------------------------------------------------------------
class	vulkan_window
		: public window_android<window>
{
public:
	using	super_type	= window_android<window>;

	vulkan_window()
	{
		mDevice	= new android_vkdevice();
	}

	virtual context_type get_context() const noexcept override
	{
		return	nullptr;
	}

	virtual bool repaint(std::function<void()> _repevent) override
	{
		_repevent();
		return	true;
	}

	virtual amtrs::g3d::device* get_g3d_device() override
	{
		return	mDevice;
	}

	virtual void on_initialize() override
	{
		AMTRS_JAVA_JNIMETHOD_REGISTER(SurfaceView, vulkan_window,
			AMTRS_JAVA_MAKE_JNIMETHOD(surfaceChanged),
			AMTRS_JAVA_MAKE_JNIMETHOD(surfaceCreated),
			AMTRS_JAVA_MAKE_JNIMETHOD(surfaceDestroyed),
			AMTRS_JAVA_MAKE_JNIMETHOD(onTouchEvent),
			AMTRS_JAVA_MAKE_JNIMETHOD(dispatchKeyEvent)
		);
 

		auto	activity		= os::android::get_activity();
		auto	clsSurface		= SurfaceView::find();
		auto	surface			= clsSurface.init(activity, static_cast<jlong>(reinterpret_cast<std::intptr_t>(this)));
		if (!surface.get())
		{
			AMTRS_ERROR_LOG("SurfaceVieww initialize error");
			throw	std::runtime_error("SurfaceVieww initialize error.");
		}
		surface.getHolder().setFormat(PixelFormat::find().get_RGBX_8888());

		activity.pushView(java::jobj<java::classes::android::view::View>(surface));
		mSurface	= surface;

	}



	void surfaceChanged(java::jobj<SurfaceHolder> _holder, int format, int width, int height)
	{
		AMTRS_INFO_LOG("surfaceChanged format=%d, %dx%d", format, width, height);
		mDevice->mCanvasSize	= { width, height };
	}

	void surfaceCreated(java::jobj<SurfaceHolder> _holder)
	{
		AMTRS_INFO_LOG("surfaceCreated");
		mDevice->mWindow	= ANativeWindow_fromSurface(java::get_jnienv(), _holder.getSurface().get());
		mDevice->initialize();
	}

	void surfaceDestroyed(java::jobj<SurfaceHolder> _holder)
	{
		AMTRS_INFO_LOG("surfaceDestroyed");
		mDevice->clean();
		mDevice->mWindow	= nullptr;
	}


	ref<android_vkdevice>		mDevice;
	java::globalref<jobject>	mSurface;
};


ref<window> __android__create_vulkan_window(window::size_type _size, create_params const& _cp)
{
	ref<vulkan_window>	thiz	= new vulkan_window();
	thiz->initialize();
	return	thiz;
}

AMTRS_G3D_NAMESPACE_END
