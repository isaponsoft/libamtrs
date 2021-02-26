/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkbarrier__hpp
#define	__libamtrs__g3d__vulkan__vkbarrier__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


// ****************************************************************************
//! Pipeline Barrier Util
// ----------------------------------------------------------------------------
//!	exqmple)
//!		barrier	b(commandBuffer);
//!		b.present_barrier(swapchain);
//!		....
// ----------------------------------------------------------------------------
class	vkbarrier
{
public:
	vkbarrier() = default;
	vkbarrier(vkbarrier const&) = default;
	vkbarrier(vkbarrier&&) = default;
	vkbarrier& operator = (vkbarrier const&) = default;
	vkbarrier& operator = (vkbarrier&&) = default;

	static vkbarrier create(vkref<VkCommandBuffer> const& _cmd)
	{
		return	vkbarrier(_cmd);
	}


	vkbarrier(vkref<VkCommandBuffer> const& _cmd)
		: mCmd(_cmd)
	{}
/*
	// ========================================================================
	//! スワップチェーンのプレセントに対するバリアを挿入します。
	// ------------------------------------------------------------------------
	vkbarrier& present(vkref<VkSwapchainKHR> const& _swapchain)
	{
		return	present
				(
					std::get<VkImage>(_swapchain.current_image_view()),
					_swapchain.graphics_family(),
					_swapchain.present_family()
				);
	}
*/
	// ========================================================================
	//! イメージのプレセントに対するバリアを挿入します。
	// ------------------------------------------------------------------------
	//! present(vkref<VkSwapchainKHR> const& _swapchain) から呼ばれます。
	// ------------------------------------------------------------------------
	vkbarrier& present(VkImage _swapchainImage, uint32_t _graphicsQueue, uint32_t _presentQueue)
	{
		VkImageSubresourceRange subResourceRange = {};
		subResourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		subResourceRange.baseMipLevel	= 0;
		subResourceRange.levelCount		= 1;
		subResourceRange.baseArrayLayer	= 0;
		subResourceRange.layerCount		= 1;

		VkImageMemoryBarrier	imb{};
		imb.sType				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imb.srcAccessMask		= 0;
		imb.dstAccessMask		= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imb.oldLayout			= VK_IMAGE_LAYOUT_UNDEFINED;
		imb.newLayout			= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		imb.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imb.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		if (_presentQueue == _graphicsQueue)
		{
			imb.srcQueueFamilyIndex = _presentQueue;
			imb.dstQueueFamilyIndex = _graphicsQueue;
		}
		imb.image				= _swapchainImage;
		imb.subresourceRange	= subResourceRange;
		vkCmdPipelineBarrier(mCmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &imb);
		return	*this;
	}

protected:
	vkref<VkCommandBuffer>	mCmd;
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
