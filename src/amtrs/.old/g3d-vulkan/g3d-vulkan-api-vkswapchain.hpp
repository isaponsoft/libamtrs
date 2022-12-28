/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkswapchain__hpp
#define	__libamtrs__g3d__vulkan__vkswapchain__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


template<>
struct	vkhandle_traits<VkSwapchainKHR>
{
	using	value_type	= VkSwapchainKHR;

	// No need destroy.
	void destroy(VkDevice _device, VkSurfaceKHR _surface, value_type _handle)
	{
		vkDestroySwapchainKHR(_device, _handle, nullptr);
	}

	VkFormat					format;
	vkarray<vkref<VkImageView>>	imageViews;
	uint32_t					familyIndex;
	uint32_t					presentFamilyIndex;
	uint32_t					frameIndex			= 0;
};


template<>
class	vkref<VkSwapchainKHR>
		: public vkref_impl<VkSwapchainKHR, VkDevice, VkSurfaceKHR>
{
	using	super_type	= vkref_impl<VkSwapchainKHR, VkDevice, VkSurfaceKHR>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }

	using	famly_index_type	= vkref<VkDevice>::famly_index_type;

	static constexpr famly_index_type	nidx = UINT32_MAX;


	uint32_t frame_index() const noexcept { return traits().frameIndex; }

	vkref<VkImageView> const& image_view(int _index) const noexcept { return image_views()[_index]; }
	vkarray<vkref<VkImageView>> const& image_views() const noexcept { return traits().imageViews; }
	vkref<VkImageView> const& current_image_view() const noexcept { return image_views()[traits().frameIndex]; }

	famly_index_type family_index() const noexcept { return traits().familyIndex; }

	VkFormat format() const noexcept { return traits().format; }


	VkExtent2D size() const noexcept
	{
		auto	surface	= std::get<VkSurfaceKHR>(*this);
		auto	gpu		= std::get<VkPhysicalDevice>(std::get<VkDevice>(*this));
		return	surface.size(gpu);
	}

	VkResult next_image(vkref<VkSemaphore> const& _semaphore, vkref<VkFence> const& _fence = {})
	{
		if (vklasterr) { return	vklasterr; }
		_AMTRS_VKINVOKE_R(vkAcquireNextImageKHR(std::get<VkDevice>(*this), get(), UINT64_MAX, _semaphore.get(), _fence.get(), &traits().frameIndex), vklasterr);
		return	vklasterr;
	}



	// ==============================================================
	//! スワップチェーンを作成します。
	// --------------------------------------------------------------
	static vkref<VkSwapchainKHR> create(vkref<VkDevice> const& _device, vkref<VkSurfaceKHR> const& _surface)
	{
		auto		gpu			= std::get<VkPhysicalDevice>(_device);
		uint32_t	present_queue_family_index		= UINT32_MAX;
		uint32_t	graphics_queue_family_index	= UINT32_MAX;

		uint32_t								queue_family_count;
		std::vector<VkQueueFamilyProperties>	queue_props;
		vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, nullptr);

		queue_props.resize(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, queue_props.data());


		std::vector<VkBool32>	supportPresents;
		supportPresents.resize(queue_family_count);
		for (uint32_t i = 0; i < queue_family_count; ++i)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, _surface, &supportPresents[i]);
		}


		for (uint32_t i = 0; i < queue_family_count; ++i)
		{
			if ((queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
			{
				if (graphics_queue_family_index == UINT32_MAX)
				{
					graphics_queue_family_index = i;
				}
				if (supportPresents[i] == VK_TRUE)
				{
					graphics_queue_family_index	= i;
					present_queue_family_index	= i;
					break;
				}
			}
		}
		if (present_queue_family_index == UINT32_MAX)
		{
			for (uint32_t i = 0; i < queue_family_count; ++i)
			{
				if (supportPresents[i] == VK_TRUE)
				{
					present_queue_family_index = i;
					break;
				}
			}
		}

		if (vklasterr) { return {}; }
		return	create(_device, _surface, {present_queue_family_index, graphics_queue_family_index});
	}


	// ==============================================================
	//! スワップチェーンを作成します。
	// --------------------------------------------------------------
	static vkref<VkSwapchainKHR> create(vkref<VkDevice> const& _device, vkref<VkSurfaceKHR> const& _surface, std::initializer_list<uint32_t> _family)
	{
		if (vklasterr) { return {}; }

		std::vector<uint32_t>		queueFamilyIndices(_family);

		VkPhysicalDevice			gpu		= std::get<VkPhysicalDevice>(_device);
		VkFormat					format	= _surface.format(gpu);
		VkSurfaceCapabilitiesKHR	caps	= _surface.caps(gpu);


		VkSwapchainCreateInfoKHR	ci
		{
			.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext					= nullptr,
			.surface				= _surface,
			.minImageCount			= caps.minImageCount,
			.imageFormat			= format,
			.imageColorSpace		= VK_COLORSPACE_SRGB_NONLINEAR_KHR,
			.imageExtent			= caps.currentExtent,
			.imageArrayLayers		= 1,
			.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.imageSharingMode		= VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount	= 0,
			.pQueueFamilyIndices	= nullptr,
			.preTransform			= caps.currentTransform,
			.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode			= VK_PRESENT_MODE_FIFO_KHR,
			.clipped				= true,
			.oldSwapchain			= nullptr,
		};

		if (ci.imageExtent.width == UINT32_MAX)
		{
			ci.imageExtent.width		= ci.imageExtent.width < caps.minImageExtent.width ? ci.imageExtent.width : caps.minImageExtent.width;
			ci.imageExtent.width		= ci.imageExtent.width > caps.minImageExtent.width ? ci.imageExtent.width : caps.minImageExtent.width;
		}
		if (ci.imageExtent.height == UINT32_MAX)
		{
			ci.imageExtent.height		= ci.imageExtent.height < caps.minImageExtent.height ? ci.imageExtent.height : caps.minImageExtent.height;
			ci.imageExtent.height		= ci.imageExtent.height > caps.minImageExtent.height ? ci.imageExtent.height : caps.minImageExtent.height;
		}

		VkCompositeAlphaFlagBitsKHR	compositeAlphaFlags[] = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (VkCompositeAlphaFlagBitsKHR flg : compositeAlphaFlags)
		{
			if (caps.supportedCompositeAlpha & flg)
			{
				ci.compositeAlpha = flg;
				break;
			}
		}

		VkSwapchainKHR			handle	= VK_NULL_HANDLE;
		_AMTRS_VKINVOKE(vkCreateSwapchainKHR(_device, &ci, nullptr, &handle));
		vkref<VkSwapchainKHR>	retval(handle, _device, _surface);
		retval.initialize(ci.imageExtent, format);
		retval.traits().familyIndex			= queueFamilyIndices[0];
		return	retval;
	}

	void reset_imageviews(VkExtent2D _size)
	{
		traits().imageViews.clear();
		initialize(_size, traits().format);
	}

protected:
	void initialize(VkExtent2D _size, VkFormat _format)
	{
		vkref<VkDevice>	dev		= std::get<VkDevice>(*this);
		VkSwapchainKHR	sc		= get();
		auto			images	= vkenum<VkImage>([&](auto* _count, auto _data) {
									return	vkGetSwapchainImagesKHR(dev, sc, _count, _data);
								});
		if (vklasterr)
		{
			return;
		}
		auto&		t		= traits();
		t.format		= _format;
		t.imageViews	= decltype(t.imageViews)(images.size());
		auto	ivit	= t.imageViews.begin();
		for (auto& i : images)
		{
			auto	img		= vkref<VkImage>::create_no_destroy(i, dev, _size, _format);
			*ivit++	= vkref<VkImageView>::create(img);
		}
	}
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
