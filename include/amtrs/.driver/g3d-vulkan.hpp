/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__hpp
#define	__libamtrs__driver__g3d__vulkan__hpp
#if		AMTRS_VULKAN_USE
#include "../amtrs.hpp"


// Windows platform
#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#define	VK_USE_PLATFORM_WIN32_KHR	1


// Android platform
#elif	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_ANDROID
#define	VK_USE_PLATFORM_ANDROID_KHR	1

// Unix like platform ... X11-XCB
#elif	__has_include(<xcb/xcb.h>)
#define	VK_USE_PLATFORM_XCB_KHR		1
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <xcb/xcb.h>

// Unix like platform ... X11
#elif	__has_include(<X11/Xlib.h>)
#define	VK_USE_PLATFORM_X11_KHR		1
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// Unkown
#else


#endif	// AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_xxxx



#define	VK_NO_PROTOTYPES	1
#include <vulkan/vulkan.h>



#define	__AMTRS__VULKAN_FUNC(_name)	extern	PFN_##_name	_name;
#include "g3d-vulkan-api-function_list.hpp"
#undef	__AMTRS__VULKAN_FUNC



#ifndef	AMTRS_G3D_VKUTIL_NAMESPACE
#define	AMTRS_G3D_VKUTIL_NAMESPACE			amtrs::g3d::vulkan
#endif

#define	AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN	namespace AMTRS_G3D_VKUTIL_NAMESPACE {
#define	AMTRS_G3D_VKUTIL_NAMESPACE_END		}



#define	_AMTRS_VKINVOKE_R(_func, _falie)	\
			if constexpr (std::is_same<VkResult, decltype(_func)>::value) \
			{\
				vklasterr	= _func;\
				if (vklasterr)\
				{\
					AMTRS_WARN_LOG(#_func ": %s", std::to_string(vklasterr).c_str());\
					return	_falie;\
				}\
			} else {\
				_func;\
			}


#define	_AMTRS_VKINVOKE(_func)	_AMTRS_VKINVOKE_R(_func, {})


AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN
inline bool has_stencil(VkFormat _format)
{
	return	_format == VK_FORMAT_D32_SFLOAT_S8_UINT || _format == VK_FORMAT_D24_UNORM_S8_UINT;
}
AMTRS_G3D_VKUTIL_NAMESPACE_END


// keep order
#include <deque>
#include <list>
#include <string>
#include <tuple>
#include <vector>

// API Wrapper
#include "g3d-vulkan-api-error.hpp"
#include "g3d-vulkan-api-ref.hpp"
#include "g3d-vulkan-api-array.hpp"
#include "g3d-vulkan-api-enum.hpp"
#include "g3d-vulkan-api-vkinstance.hpp"
#include "g3d-vulkan-api-vkphysicaldevice.hpp"
#include "g3d-vulkan-api-vkdevice.hpp"
#include "g3d-vulkan-api-vkfence.hpp"
#include "g3d-vulkan-api-vksemaphore.hpp"
#include "g3d-vulkan-api-vksurface.hpp"
#include "g3d-vulkan-api-vkdevicememory.hpp"
#include "g3d-vulkan-api-vkbuffer.hpp"
#include "g3d-vulkan-api-vkimage.hpp"
#include "g3d-vulkan-api-vkimageview.hpp"
#include "g3d-vulkan-api-vkswapchain.hpp"
#include "g3d-vulkan-api-vkrenderpass.hpp"
#include "g3d-vulkan-api-vkframebuffer.hpp"
#include "g3d-vulkan-api-vkcommandpool.hpp"
#include "g3d-vulkan-api-vkcommandbuffer.hpp"
#include "g3d-vulkan-api-vkbarrier.hpp"
#include "g3d-vulkan-api-vkdescriptorsetlayout.hpp"
#include "g3d-vulkan-api-vkdescriptorpool.hpp"
#include "g3d-vulkan-api-vkdescriptorset.hpp"
#include "g3d-vulkan-api-vkpipelinecache.hpp"
#include "g3d-vulkan-api-vkpipelinelayout.hpp"
#include "g3d-vulkan-api-vkpipeline.hpp"
//#include "g3d-vulkan-api-vkpipelioneshaderstage.hpp"
#include "g3d-vulkan-api-vkqueue.hpp"
#include "g3d-vulkan-api-vkrect.hpp"
#include "g3d-vulkan-api-vksampler.hpp"
#include "g3d-vulkan-api-vkshadermodule.hpp"

// G3D Driver
#include "g3d-vulkan-vkgraphics.hpp"
#include "g3d-vulkan-vkpipelinelayout.hpp"
#include "g3d-vulkan-vkconstants.hpp"
#include "g3d-vulkan-vktexture.hpp"
#include "g3d-vulkan-vkframebuffer.hpp"
#include "g3d-vulkan-vkfunc.hpp"
#include "g3d-vulkan-vkmaterial.hpp"
#include "g3d-vulkan-vkmesh.hpp"
#include "g3d-vulkan-vkrenderstate.hpp"
#include "g3d-vulkan-vksampler.hpp"
#include "g3d-vulkan-vkshadermodule.hpp"
#include "g3d-vulkan-vkpipeline.hpp"
#include "g3d-vulkan-vkuniform.hpp"
#include "g3d-vulkan-vkvertexbuffer.hpp"
#include "g3d-vulkan-vkexecuter.hpp"
#include "g3d-vulkan-vkdevice.hpp"


#endif	// AMTRS_VULKAN_USE
#endif	// __libamtrs__driver__g3d__vulkan__hpp
