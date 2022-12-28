/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkqueue__hpp
#define	__libamtrs__g3d__vulkan__vkqueue__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


template<>
struct	vkhandle_traits<VkQueue>
{
	using	value_type	= VkQueue;

	// No need destroy.
	void destroy(VkDevice _device, value_type _handle)
	{
	}

	uint32_t				familyIndex;
};


template<>
class	vkref<VkQueue>
		: public vkref_impl<VkQueue, VkDevice>
{
	using	super_type	= vkref_impl<VkQueue, VkDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }



	// ==============================================================
	//! キューを作成します。
	// --------------------------------------------------------------
	static vkref<VkQueue> create(vkref<VkDevice> const& _device)
	{
		vkref<VkQueue>	retval;
		VkQueue			handle;
		vkGetDeviceQueue(_device, _device.family_index(), 0, &handle);
		retval	= super_type::_make(handle, _device);
		retval.traits().familyIndex	= _device.family_index();
		return	retval;
	}

	

	
	VkResult present(std::initializer_list<vkref<VkSwapchainKHR>> _swapchains)
	{
		std::vector<VkSwapchainKHR>	swaps;
		std::vector<uint32_t>		imageIndex;
		std::vector<VkResult>		results;
		swaps.resize(_swapchains.size());
		imageIndex.resize(_swapchains.size());
		results.resize(_swapchains.size());
		auto	s	= _swapchains.begin();
		for (int i = 0; i < swaps.size(); ++i)
		{
			swaps[i]		= s->get();
			imageIndex[i]	= s->frame_index();
			++s;
		}
		VkPresentInfoKHR	pi{0};
		pi.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		pi.pNext				= nullptr;
		pi.waitSemaphoreCount	= 0;
		pi.pWaitSemaphores		= nullptr;
		pi.swapchainCount		= (uint32_t)swaps.size();
		pi.pSwapchains			= swaps.data();
		pi.pImageIndices		= imageIndex.data();
		pi.pResults				= results.data();
		vklasterr	= vkQueuePresentKHR(get(), &pi);
		return	vklasterr;
	}


	VkResult submit(VkFence _fence, VkSemaphore _semaphore, VkCommandBuffer _cmd)
	{
		VkPipelineStageFlags	mask		=	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkCommandBuffer			cmnds[1]	= { _cmd };
		VkSemaphore				semps[1]	= { _semaphore };
		VkSubmitInfo			sinfo{0};
		sinfo.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		sinfo.pNext					= nullptr;
		sinfo.waitSemaphoreCount	= 1;
		sinfo.pWaitSemaphores		= semps;
		sinfo.pWaitDstStageMask		= &mask;
		sinfo.commandBufferCount	= 1;
		sinfo.pCommandBuffers		= cmnds;
		sinfo.signalSemaphoreCount	= 0;
		sinfo.pSignalSemaphores		= nullptr;
		vklasterr	= vkQueueSubmit(get(), 1, &sinfo, _fence);
		return	vklasterr;
	}


	uint32_t family_index() const noexcept { return traits().familyIndex; }
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
