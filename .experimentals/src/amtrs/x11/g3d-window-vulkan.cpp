/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <thread>
#include <chrono>
#include <deque>

#define	VK_NO_PROTOTYPES			1
#define	VK_USE_PLATFORM_XCB_KHR		1
#include <chrono>
#include <thread>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <xcb/xcb.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_xlib.h>
#include <amtrs/g3d/@>
#include <amtrs/.driver/g3d/vulkan/api/@>
#include <amtrs/.driver/g3d/vulkan/vkdevice.hpp>
#include <amtrs/.driver/x11/window-window-xcb.hpp>
#include <amtrs/g3d/device.hpp>
#include <amtrs/g3d/window.hpp>


AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN
template<>
struct	surface_creator<xcb_connection_t*, xcb_window_t>
{
	VkResult create(VkSurfaceKHR* _result, VkInstance _instance, xcb_connection_t* _conn, xcb_window_t _window)
	{
		AMTRS_WARN_LOG("Conn %p, Window %x",_conn, (unsigned int)_window);
		VkXcbSurfaceCreateInfoKHR	cio
		{
		  .sType		= VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
		  .pNext		= nullptr,
		  .flags		= 0,
		  .connection	= _conn,
		  .window		= _window,
		};
		vklasterr	= vkCreateXcbSurfaceKHR(_instance, &cio, nullptr, _result);
		AMTRS_WARN_LOG("vkCreateXcbSurfaceKHR: %s", std::to_string(vklasterr).c_str());
		return	vklasterr;
	}
};
AMTRS_G3D_VKUTIL_NAMESPACE_END


AMTRS_G3D_NAMESPACE_BEGIN
using namespace	vulkan;

class	vulkan_window;

class	x11_vkdevice
		: public vkdevice
{
public:

	//! vkdevice から呼ばれる。
	//! _initdataにプラットフォーム固有の初期化情報を格納して返す。
	virtual void on_initialize(initdata& _initdata) override
	{
		if (mInstance.empty())
		{
			std::vector<char const*>	extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME});
			mInstance	= vkref<VkInstance>::create("libamtrs", std::move(extensions), {});
		}
		_initdata.instance	= mInstance;
		AMTRS_WARN_LOG("Instance %p", _initdata.instance.get())
		_initdata.surface	= vkref<VkSurfaceKHR>::create(mInstance, mConnect, mWindow);
		AMTRS_WARN_LOG("Surface %p", _initdata.surface.get())
	}

	vkref<VkInstance>	mInstance;
	xcb_connection_t*	mConnect	= nullptr;
	xcb_window_t		mWindow		= 0;
};




// ============================================================================
//! VULKAN WINDOW
// ----------------------------------------------------------------------------
class	vulkan_window
		: public window_xcb<window>
{
public:
	using	super_type	= window_xcb<window>;

	vulkan_window()
	{
	}

	virtual void on_xevent(xcb_configure_notify_event_t* _event) override
	{
		auto	ps	= this->size();
		super_type::on_xevent(_event);
		if (ps != this->size())
		{
			mDevice->resize(this->size());
		}
	}

	void initialize(window::size_type _size)
	{
		super_type::initialize(_size);

		mDevice	= new x11_vkdevice();
		mDevice->mConnect	= super_type::get_xcb_connection();
		mDevice->mWindow	= super_type::get_xcb_window();
		mDevice->resize(_size);
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


	ref<x11_vkdevice>			mDevice;
};


ref<window> __x11xcb__create_vulkan_window(window::size_type _size, create_params const& _cp)
{
	ref<vulkan_window>	thiz	= new vulkan_window();
	thiz->initialize(_size);
	return	thiz;
}

AMTRS_G3D_NAMESPACE_END
