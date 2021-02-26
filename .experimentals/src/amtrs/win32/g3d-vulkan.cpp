/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#if		AMTRS_VULKAN_USE
#include <thread>
#include <chrono>
#include <deque>

#define	VK_NO_PROTOTYPES				1
#define	VK_USE_PLATFORM_WIN32_KHR		1
#include <amtrs/g3d.hpp>
#include <amtrs/.driver/g3d-vulkan.hpp>
#include <amtrs/.driver/win32-window-window.hpp>


AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN
template<>
struct	surface_creator<HINSTANCE, HWND>
{
	VkResult create(VkSurfaceKHR* _result, VkInstance _instance, HINSTANCE _hinstance, HWND _window)
	{
		VkWin32SurfaceCreateInfoKHR 	cio
		{
		  .sType		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		  .pNext		= nullptr,
		  .flags		= 0,
		  .hinstance	= _hinstance,
		  .hwnd			= _window,
		};
		vklasterr	= vkCreateWin32SurfaceKHR(_instance, &cio, nullptr, _result);
		AMTRS_WARN_LOG("vkCreateXcbSurfaceKHR: %s", std::to_string(vklasterr).c_str());
		return	vklasterr;
	}
};
AMTRS_G3D_VKUTIL_NAMESPACE_END


AMTRS_G3D_NAMESPACE_BEGIN


ref<device> device::create(api_type _type, window* _window)
{
	using namespace	vulkan;

	return	new vkdevice(_window, [](auto* _window, auto& _initdata)
			{
				auto*		win			= static_cast<window_win32*>(_window);
				HINSTANCE	hInstance	= GetModuleHandle(nullptr);
				HWND		hwnd		= win->hwnd();
				std::vector<char const*>	extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME});
				_initdata.instance	= vkref<VkInstance>::create("libamtrs", std::move(extensions), {"VK_LAYER_KHRONOS_validation"});
				_initdata.surface	= vkref<VkSurfaceKHR>::create(_initdata.instance, hInstance, hwnd);
			});
}



AMTRS_G3D_NAMESPACE_END
#endif

