/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__x11__g3d_window_vulkan__hpp
#define	__libamtrs__driver__x11__g3d_window_vulkan__hpp

#define	VK_USE_PLATFORM_XCB_KHR
#include <chrono>
#include <thread>
#include <xcb/xcb.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_xlib.h>
#include "../g3d/vulkan/api/def.hpp"
#include "../g3d/vulkan/api/error.hpp"
#include "../g3d/vulkan/api/vkcommandbuffer.hpp"
#include "../g3d/vulkan/api/vkcommandpool.hpp"
#include "../g3d/vulkan/api/vkframebuffer.hpp"
#include "../g3d/vulkan/api/vkqueue.hpp"
#include "../g3d/vulkan/api/vkrenderpass.hpp"
#include "../g3d/vulkan/api/vkswapchain.hpp"

AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN

template<>
struct	surface_creator<xcb_connection_t*, xcb_window_t>
{
	static VkResult create(VkSurfaceKHR* _result, VkInstance _instance, xcb_connection_t* const& _conn, xcb_window_t const& _window)
	{
		VkDisplayKHR				display;
		uint32_t					planeIndex;
		*_result	= VK_NULL_HANDLE;

		VkXcbSurfaceCreateInfoKHR	ci	= {};
		ci.sType		= VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		ci.connection	= _conn;
		ci.window		= _window;
		vklasterr		= vkCreateXcbSurfaceKHR(_instance, &ci, nullptr, _result);
		if (vklasterr)
		{
			AMTRS_DEBUG_LOG("vkCreateXlibSurfaceKHR %s", std::to_string(vklasterr).c_str());
			return	vklasterr;
		}
		return	vklasterr;
	}
};

AMTRS_G3D_VKUTIL_NAMESPACE_END
AMTRS_G3D_NAMESPACE_BEGIN

class	window_xcb_vulkan
		: public window_xcb<window>
{
public:
	using	super_type			= window_xcb<window>;


	class	device_xcb_vulkan
			: public g3d::device
	{
	public:
		using	super_type	= g3d::device;

		device_xcb_vulkan(window_xcb_vulkan* _window)
			: super_type(super_type::vulkan)
			, mWindow(_window)
		{}

		//! コンテキストが有効か調べます。
		virtual bool is_context_enable() const override
		{
			return	false;
		}

		virtual std::string name() const override
		{
			return	"vulkan with xcb";
		}

		virtual typename g3d::device::size_type size() const override
		{
			return	mWindow->size();
		}

		virtual void active() override
		{
		}

		virtual void deactive() override
		{
		}

		virtual void draw(g3d::draw_command* _command) override
		{
		}

		virtual ref<g3d::shader> create_shader(const attribute* _attr, std::size_t _attr_size, const attribute* _uniform, std::size_t _uniform_size, std::string_view _vertex, std::string_view _fragment) override
		{
			return	{};
		}

		bool initialize()
		{
			using namespace vulkan;

			static char const*		exts[]	= { VK_KHR_XCB_SURFACE_EXTENSION_NAME };
			mInstance	= vkref<VkInstance>::create("amtrs", exts, 1);
			if (vklasterr)
			{
				AMTRS_DEBUG_LOG("Vulkan create instance error.", std::to_string(vklasterr).c_str());
				return	false;
			}
			mGPU		= vkref<VkPhysicalDevice>::create(mInstance);
			if (vklasterr)
			{
				AMTRS_DEBUG_LOG("Vulkan create pyhisicaldevice error.", std::to_string(vklasterr).c_str());
				return	false;
			}
			mSurface	= vkref<VkSurfaceKHR>::create(mInstance, mWindow->get_xcb_connection(), mWindow->get_xcb_window());
			if (vklasterr)
			{
				AMTRS_DEBUG_LOG("Vulkan create surface error.", std::to_string(vklasterr).c_str());
				return	false;
			}

			auto	fam	= mGPU.get_queue_families(mSurface);
			mPresentFamilyIndex		= fam.first;
			mGraphicsFamilyIndex	= fam.second;
			mDevice				= vkref<VkDevice>::create(mGPU, mGraphicsFamilyIndex);

			mPresentSemaphore	= vulkan::vkref<VkSemaphore>::create(mDevice);
			return	true;
		}


		bool next_image()
		{
			using namespace vulkan;
			if (mSwapchain.empty())
			{
				mSwapchain		= vkref<VkSwapchainKHR>::create(mDevice, mSurface, mPresentFamilyIndex, mGraphicsFamilyIndex);
				if (vklasterr)
				{
					AMTRS_DEBUG_LOG("Vulkan create swapchain error.", std::to_string(vklasterr).c_str());
					return	false;
				}
			}
			if (mSwapchain.next_image(mPresentSemaphore))
			{
				return	false;
			}
			return	true;
		}


		virtual void present() override
		{
			using namespace vulkan;
			vkref<VkCommandBuffer>::create(mDevice)
				.begin(mSwapchain, [&](vkref<VkCommandBuffer>& cmdbuff)
				{
					VkCommandBuffer	c	= cmdbuff;
					VkFramebuffer	f	= cmdbuff.last_framebuffer();

					cmdbuff.renderpass({0,0,1,1}, [&](vkref<VkCommandBuffer>& cmdbuff)
					{
					});
				})
				.submit(mPresentSemaphore, vkref<VkFence>::create(mDevice))
			;
			mSwapchain.present(mDevice.queue());
		}


		window_xcb_vulkan*				mWindow;
		uint32_t						mPresentFamilyIndex;
		uint32_t						mGraphicsFamilyIndex;
		vulkan::vkref<VkInstance>		mInstance;
		vulkan::vkref<VkPhysicalDevice>	mGPU;
		vulkan::vkref<VkSurfaceKHR>		mSurface;
		vulkan::vkref<VkDevice>			mDevice;

		vulkan::vkref<VkSwapchainKHR>	mSwapchain;
		vulkan::vkref<VkSemaphore>		mPresentSemaphore;
	};


	void initialize(size_type _size)
	{
		using namespace vulkan;
		super_type::initialize(_size);

		mDevice		= new device_xcb_vulkan(this);
		if (!mDevice->initialize())
		{
			mDevice.clear();
		}
	}


	virtual bool repaint(std::function<void()> _repevent) override
	{
		if (!mDevice->next_image())
		{
			return	false;
		}
		return	super_type::repaint(std::move(_repevent));
	}




	virtual g3d::device* get_g3d_device() override
	{
		return	mDevice;
	}


	ref<device_xcb_vulkan>	mDevice;
};



AMTRS_G3D_NAMESPACE_END
#endif
