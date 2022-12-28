/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkcommandbuffer__hpp
#define	__libamtrs__g3d__vulkan__vkcommandbuffer__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


inline VkAccessFlags make_access_flags(VkImageLayout _layout)
{
	switch (_layout)
	{
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL :			return	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL :				return	VK_ACCESS_TRANSFER_WRITE_BIT;
		case VK_IMAGE_LAYOUT_PREINITIALIZED :					return	VK_ACCESS_HOST_WRITE_BIT;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL :			return	VK_ACCESS_SHADER_READ_BIT;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL :				return	VK_ACCESS_TRANSFER_READ_BIT;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL :	return	VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:					return	VK_ACCESS_MEMORY_READ_BIT;
		default :												return	VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
	}
}


template<>
struct	vkhandle_traits<VkCommandBuffer>
{
	using	value_type	= VkCommandBuffer;

	// No need destroy.
	void destroy(vkref<VkCommandPool> const& _pool, value_type _handle)
	{
		vkFreeCommandBuffers(std::get<VkDevice>(_pool), _pool, 1, &_handle);
	}
};


template<>
class	vkref<VkCommandBuffer>
		: public vkref_impl<VkCommandBuffer, VkCommandPool>
{
	using	super_type	= vkref_impl<VkCommandBuffer, VkCommandPool>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }

	static vkref<VkCommandBuffer> create(vkref<VkCommandPool> const& _pool, VkCommandBufferLevel _level = VK_COMMAND_BUFFER_LEVEL_PRIMARY)
	{
		if (vklasterr) { return {}; }
		VkCommandBufferAllocateInfo	ai{};
		ai.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		ai.pNext				= nullptr;
		ai.commandPool			= _pool;
		ai.level				= _level;
		ai.commandBufferCount	= 1;

		VkCommandBuffer	handle	= VK_NULL_HANDLE;
		vklasterr	= vkAllocateCommandBuffers(std::get<VkDevice>(_pool), &ai, &handle);
		return	super_type::_make(handle, _pool);
	}

	vkref& begin()
	{
		if (vklasterr) { return *this; }
		VkCommandBufferBeginInfo	bi{};
		bi.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		bi.pNext				= nullptr;
		bi.flags				= 0;
		bi.pInheritanceInfo		= nullptr;
		vklasterr	= vkBeginCommandBuffer(get(), &bi);
		return	*this;
	}	

	vkref& end()
	{
		if (vklasterr) { return *this; }
		vklasterr	= vkEndCommandBuffer(get());
		return	*this;
	}	



	vkref& begin_renderpass(vkref<VkRenderPass> _renderpass, vkref<VkFramebuffer> _framebuff, VkClearValue _color)
	{
		VkClearValue	cb[2]{0};
		cb[0]	= _color;
		cb[1].depthStencil	= {1.0f,0};

		VkRenderPassBeginInfo	rbi{0};
		rbi.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rbi.pNext				= nullptr;
		rbi.renderPass			= _renderpass;
		rbi.framebuffer			= _framebuff;
		rbi.renderArea.offset.x	= 0;
		rbi.renderArea.offset.y	= 0;
		rbi.renderArea.extent	= _framebuff.size();
		rbi.clearValueCount		= 2;
		rbi.pClearValues		= cb;
		vkCmdBeginRenderPass(get(), &rbi, VK_SUBPASS_CONTENTS_INLINE);
		return	*this;
	}

	vkref& end_renderpass()
	{
		vkCmdEndRenderPass(get());
		return	*this;
	}


	vkref& pipeline_barrier(VkImage _image, VkImageLayout _startLayout, VkPipelineStageFlags _startStage, VkImageLayout _afterLayout, VkPipelineStageFlags _afterStage)
	{
		VkImageMemoryBarrier	imb;
		imb.sType					= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imb.pNext					= NULL;
		imb.srcAccessMask			= make_access_flags(_startLayout);
		imb.dstAccessMask			= make_access_flags(_afterLayout);
		imb.oldLayout				= _startLayout;
		imb.newLayout				= _afterLayout;
		imb.srcQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
		imb.dstQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
		imb.image					= _image;
		imb.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		imb.subresourceRange.baseMipLevel	= 0;
		imb.subresourceRange.levelCount		= 1;
		imb.subresourceRange.baseArrayLayer	= 0;
		imb.subresourceRange.layerCount		= 1;
		vkCmdPipelineBarrier(get(), _startStage, _afterStage, 0, 0, nullptr, 0, nullptr, 1, &imb);
		return	*this;
	}


#if	0
	vkref<VkFramebuffer> last_framebuffer() const noexcept { return traits().framebuffer; }

/*
	template<class CodeBlock>
	vkref& begin(vkref<VkSwapchainKHR> const& _swapchain, CodeBlock&& _block)
	{
		if (vklasterr) { return *this; }
		auto	framebuffer	= vkref<VkFramebuffer>::create(_swapchain);
		begin(framebuffer, std::forward<CodeBlock>(_block));
		return	*this;
	}

	vkref& begin(vkref<VkSwapchainKHR> const& _swapchain)
	{
		if (vklasterr) { return *this; }
		auto	framebuffer	= vkref<VkFramebuffer>::create(_swapchain);
		begin(framebuffer);
		return	*this;
	}
*/
	template<class CodeBlock>
	vkref& begin(vkref<VkFramebuffer> const& _framebuffer, CodeBlock&& _block)
	{
		if (vklasterr) { return *this; }
		begin(_framebuffer);
		_block(static_cast<vkref<VkCommandBuffer>&>(*this));
		end();
		return	*this;
	}

	vkref& begin(vkref<VkFramebuffer> const& _framebuffer)
	{
		if (vklasterr) { return *this; }
		VkCommandBufferInheritanceInfo 	ii{};
		ii.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		ii.framebuffer			= _framebuffer;
		VkCommandBufferBeginInfo		bi{};
		bi.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		bi.pInheritanceInfo		= &ii;
		vkBeginCommandBuffer(get(), &bi);

		traits().framebuffer	= _framebuffer;
		return	*this;
	}

	template<class CodeBlock>
	vkref& begin(VkCommandBufferUsageFlags _usageFlags, CodeBlock&& _code)
	{
		if (vklasterr) { return *this; }
		begin(_usageFlags);
		_code(static_cast<vkref<VkCommandBuffer>&>(*this));
		end();
		return	*this;
	}

	vkref& begin(VkCommandBufferUsageFlags _usageFlags = 0)
	{
		if (vklasterr) { return *this; }
		VkCommandBufferBeginInfo		bi{};
		bi.sType	= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		bi.flags	= _usageFlags;
		vkBeginCommandBuffer(get(), &bi);
		return	*this;
	}

	vkref& end()
	{
		if (vklasterr) { return *this; }
		vkEndCommandBuffer(get());
		return	*this;
	}

/*
	template<class CodeBlock>
	vkref& renderpass(VkClearColorValue _color, CodeBlock&& _block)
	{
		if (vklasterr) { return *this; }
		begin_renderpass(_color, traits().framebuffer);
		_block(static_cast<vkref<VkCommandBuffer>&>(*this));
		end_renderpass();
		return	*this;
	}
*/

	vkref& begin_renderpass(VkClearColorValue _color, VkExtent2D _renderArea, VkFramebuffer _framebuff, VkRenderPass _renderpass)
	{
		if (vklasterr) { return *this; }
		VkClearValue			cv;
		cv.color		= _color;
		cv.depthStencil	= {0, 0};

		VkExtent2D				size	= _renderArea;//std::get<VkSwapchainKHR>(traits().framebuffer).extent2d();

		VkRenderPassBeginInfo	rpbi{};
		rpbi.sType						= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rpbi.framebuffer				= _framebuff ? _framebuff : traits().framebuffer;
		rpbi.renderPass					= _renderpass ? _renderpass : std::get<VkRenderPass>(traits().framebuffer);
		rpbi.renderArea.extent.width	= size.width;
		rpbi.renderArea.extent.height	= size.height;
		rpbi.clearValueCount			= 1;
		rpbi.pClearValues				= &cv;
		vkCmdBeginRenderPass(get(), &rpbi, VK_SUBPASS_CONTENTS_INLINE);
		return	*this;
	}


	vkref& end_renderpass()
	{
		if (vklasterr) { return *this; }
		vkCmdEndRenderPass(get());
		return	*this;
	}

	vkref& submit(vkref<VkSemaphore> const& _semaphore, vkref<VkFence> const& _fence)
	{
		if (vklasterr) { return *this; }
		auto			device	= std::get<VkDevice>(std::get<VkCommandPool>(*this));
		VkFence				fence		= _fence;
		VkSemaphore			semaphore	= _semaphore.get();
		VkCommandBuffer		cmdbuff[]	= { get() };

		VkSubmitInfo			si{};
		VkPipelineStageFlags	waitStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

		si.sType 				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		si.waitSemaphoreCount	= 1;
		si.pWaitSemaphores		= &semaphore;
		si.pCommandBuffers		= cmdbuff;
		si.pWaitDstStageMask	= &waitStageMask;
		si.commandBufferCount	= 1;
		vkQueueSubmit(device.queue(), 1, &si, fence);
		vkWaitForFences(device, 1, &fence, VK_FALSE, UINT64_MAX);
		return	*this;
	}


	vkref& copy(vkref<VkBuffer> const& _dest, vkref<VkBuffer> const& _src)
	{
		if (vklasterr) { return *this; }
		VkBufferCopy	bc{};
		bc.size		= _dest.size();
		vkCmdCopyBuffer(get(), _src, _dest, 1, &bc);
		return	*this;
	}


	// ==============================================================
	//! コマンドプールからコマンドバッファを作成します。
	// --------------------------------------------------------------
	//! コマンドプールはスレッドセーフではありません。
	// --------------------------------------------------------------
	static vkref<VkCommandBuffer> create(vkref<VkCommandPool> const& _pool, VkCommandBufferLevel _level = VK_COMMAND_BUFFER_LEVEL_PRIMARY)
	{
		if (vklasterr) { return {}; }
		VkCommandBufferAllocateInfo	ai{};
		ai.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		ai.pNext				= nullptr;
		ai.commandPool			= _pool;
		ai.level				= _level;
		ai.commandBufferCount	= 1;

		VkCommandBuffer	handle	= VK_NULL_HANDLE;
		_AMTRS_VKINVOKE(vkAllocateCommandBuffers(std::get<VkDevice>(_pool), &ai, &handle));
		return	super_type::_make(handle, _pool);
	}


	// ==============================================================
	//! デバイスからコマンドバッファを作成します。
	// --------------------------------------------------------------
	//! VkDeviceからVkCommandPoolを作成し、そのプールからバッファを
	//! 作成します。コマンドバッファが複数必要な場合は必要に応じて
	//! 自分でプールを確保したほうが処理効率が上がります。
	// --------------------------------------------------------------
	static vkref<VkCommandBuffer> create(vkref<VkDevice> const& _device, VkCommandBufferLevel _level = VK_COMMAND_BUFFER_LEVEL_PRIMARY)
	{
		if (vklasterr) { return {}; }
		return	create(vkref<VkCommandPool>::create(_device), _level);
	}
#endif
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
