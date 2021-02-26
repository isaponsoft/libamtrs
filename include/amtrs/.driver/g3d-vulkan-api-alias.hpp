/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__alias__hpp
#define	__libamtrs__g3d__vulkan__alias__hpp
#include "ref.hpp"
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN
class	vkbarrier;
namespace aliases {


using	vkbuffer				= vkref<VkBuffer>;
using	vkcommandbuffer			= vkref<VkCommandBuffer>;
using	vkcommandpool			= vkref<VkCommandPool>;
using	vkdebugreport			= vkref<VkDebugReportCallbackEXT>;
using	vkdescriptorsetlayout	= vkref<VkDescriptorSetLayout>;
using	vkdevice				= vkref<VkDevice>;
using	vkdevicememory			= vkref<VkDeviceMemory>;
using	vkfence					= vkref<VkFence>;
using	vkframebuffer			= vkref<VkFramebuffer>;
using	vkimage					= vkref<VkImage>;
using	vkimageview				= vkref<VkImageView>;
using	vkinstance				= vkref<VkInstance>;
using	vkphysicaldevice		= vkref<VkPhysicalDevice>;
using	vkpipeline				= vkref<VkPipeline>;
using	vkpipelinelayout		= vkref<VkPipelineLayout>;
using	vkqueue					= vkref<VkQueue>;
using	vkrenderpass			= vkref<VkRenderPass>;
using	vksemaphore				= vkref<VkSemaphore>;
using	vkshadermodule			= vkref<VkShaderModule>;
using	vksurface				= vkref<VkSurfaceKHR>;
using	vkswapchain				= vkref<VkSwapchainKHR>;


using	vkbarrier				= vkbarrier;

}
AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
