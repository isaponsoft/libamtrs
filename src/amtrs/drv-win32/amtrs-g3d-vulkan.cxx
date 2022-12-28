/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <amtrs/graphics/g3d/vulkan/g3d-vulkan.hpp>
#include "amtrs-g3d.hpp"

AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN
template<>
struct	surface_creator<HINSTANCE, HWND>
{
	VkResult create(VkSurfaceKHR* _result, VkInstance _instance, HINSTANCE _hinstance, HWND _window)
	{
		VkWin32SurfaceCreateInfoKHR 	cio
		{
			/*.sType		=*/VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			/*.pNext		=*/nullptr,
			/*.flags		=*/0,
			/*.hinstance	=*/_hinstance,
			/*.hwnd			=*/_window,
		};
		vklasterr	= vkCreateWin32SurfaceKHR(_instance, &cio, nullptr, _result);
		if (!vklasterr.good())
		{
			AMTRS_WARN_LOG("vkCreateXcbSurfaceKHR: %s", std::to_string(vklasterr).c_str());
		}
		return	vklasterr;
	}
};
AMTRS_G3D_VKUTIL_NAMESPACE_END

AMTRS_OS_NAMESPACE_BEGIN
using namespace AMTRS_G3D_VKUTIL_NAMESPACE;


auto g3d_vulkan_create(g3d::device::createparams& _cp) -> ref<g3d::device>
{
	return	new vkdevice(_cp.window, [](auto* _window, auto& _initdata)
			{
				// VkInstanceは破棄されないようにする
				// device再構築時でもVkInstanceは維持するようにする。再構築しても動くがValidatorで警告扱いになる。
				static	vkref<VkInstance>	inst;
				if (inst.empty())
				{
					std::vector<char const*>	extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME});
					inst	= vkref<VkInstance>::create("libamtrs", std::move(extensions), {"VK_LAYER_KHRONOS_validation"});
				}
				_initdata.instance	= inst;

				auto*		win			= static_cast<os::window*>(_window);
				HWND		hwnd		= win->mWnd;
				HINSTANCE	hInstance	= GetModuleHandle(nullptr);
				_initdata.surface	= vkref<VkSurfaceKHR>::create(_initdata.instance, hInstance, hwnd);
			});
}
AMTRS_OS_NAMESPACE_END
