/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vkgraphics__hpp
#define	__libamtrs__driver__g3d__vulkan__vkgraphics__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


//! vulkan graphics device を扱う
class	vkgraphics
		: public ref_object
{
public:
	struct	surfaceframe
	{
		vkref<VkImageView>				image;
		vkref<VkCommandPool>			pool;
		std::deque<ref<ref_object>>		objects;		// レンダリング中に破棄されないように、使用中のオブジェクトの保持する
	};


	static ref<vkgraphics> create(vkref<VkInstance> _instance, vkref<VkSurfaceKHR> _surface)
	{
		ref<vkgraphics>	thiz	= new vkgraphics();

		thiz->mInstance	= std::move(_instance);
		thiz->mSurface	= std::move(_surface);
		if (thiz->initialize())
		{
			return	thiz;
		}
		else
		{
			return	{};
		}
	}

	void destroy_surface()
	{
		mSwapchain.clear();
		mSurface.clear();
		mFramebuffers.clear();
		mPresentFence.clear();
		mPipelineCache.clear();
		mQueue.clear();
	}

	void change_surface(vkref<VkSurfaceKHR> _surface)
	{
		mSurface	= std::move(_surface);
		if (mDevice.empty())
		{
			mDevice	= vkref<VkDevice>::create(
						mGPU,
						mGPU.find_family(mSurface, VK_QUEUE_GRAPHICS_BIT),
						{VK_KHR_SWAPCHAIN_EXTENSION_NAME}
					);
		}
		mFormat	= mSurface.choise_format(mGPU, [](auto f)
				{
					return	f.format == VK_FORMAT_R8G8B8A8_UNORM ? 1 : 0;
				});

		mSwapchain	= vkref<VkSwapchainKHR>::create(mDevice, mSurface);
		reset_framebuffer(mSurface.size(mGPU));
	}

	void reset_framebuffer(VkExtent2D _size)
	{
		uint32_t	swapsize	= 0;

		mFramebuffers.clear();
		mPresentFence.clear();
		mPipelineCache.clear();

		mSwapchain.reset_imageviews(_size);
		swapsize	= (uint32_t)mSwapchain.image_views().size();

		mFramebuffers	= vkarray<surfaceframe>(swapsize);
		for (uint32_t i = 0; i < swapsize; ++i)
		{
			mFramebuffers[i].image		= mSwapchain.image_view(i);
			mFramebuffers[i].pool		= vkref<VkCommandPool>::create(mDevice);
		}
		mNextImageSemaphore	= vkref<VkSemaphore>::create(mDevice);
		mPresentFence		= vkref<VkFence>::create(mDevice);
		mPipelineCache		= vkref<VkPipelineCache>::create(mDevice);
		mQueue				= vkref<VkQueue>::create(mDevice);
	}

	bool initialize()
	{
		mGPU	= vkref<VkPhysicalDevice>::create(mInstance, mSurface, VK_QUEUE_GRAPHICS_BIT);

//		init();
		return	true;
	}

	surfaceframe& current_image()
	{
		return	mFramebuffers[mSwapchain.frame_index()];
	}

	VkResult next_image()
	{
		mSwapchain.next_image(mNextImageSemaphore);
		if (!vklasterr.good())
		{
			return	vklasterr;
		}
		mPresentFence.reset();
		int bufferIndex	= mSwapchain.frame_index();
		mFramebuffers[bufferIndex].objects.clear();
		mNextSwapchain	= bufferIndex;
		mFramebuffers[bufferIndex];
		return	VK_SUCCESS;
	}

	void submit(VkCommandBuffer _cmd)
	{
		mQueue.submit(mPresentFence, mNextImageSemaphore, _cmd);
	}

	void present()
	{
		mPresentFence.wait();
		mQueue.present({mSwapchain});
	}

	VkExtent2D surface_size()
	{
		return	mSurface.caps(mGPU).currentExtent;
	}

	vkref<VkDevice>& device()
	{
		return	mDevice;
	}

	uint32_t swapchain_size()
	{
		uint32_t	swapsize;
		vkGetSwapchainImagesKHR(mDevice, mSwapchain, &swapsize, nullptr);
		return	swapsize;
	}

	vkref<VkPipelineCache> const& pipeline_cache()
	{
		return	mPipelineCache;
	}

	int next_swapchain()
	{
		return	mNextSwapchain;
	}

	vkref<VkSwapchainKHR> const& swapchain() const noexcept { return mSwapchain; }

	vkref<VkQueue> queue() const noexcept { return mQueue; }

protected:
	vkref<VkInstance>		mInstance;
	vkref<VkSurfaceKHR>		mSurface;
	VkSurfaceFormatKHR		mFormat;
	vkref<VkPhysicalDevice>	mGPU;
	vkref<VkDevice>			mDevice;
	vkref<VkQueue>			mQueue;
	vkref<VkSwapchainKHR>	mSwapchain;
	vkref<VkSemaphore>		mNextImageSemaphore;
	vkref<VkFence>			mPresentFence;
	vkarray<surfaceframe>	mFramebuffers;
	vkref<VkPipelineCache>	mPipelineCache;
	int						mNextSwapchain	= 0;
};

AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
