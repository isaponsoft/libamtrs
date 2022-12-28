/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkimageview__hpp
#define	__libamtrs__g3d__vulkan__vkimageview__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


template<>
struct	vkhandle_traits<VkImageView>
{
	using	value_type	= VkImageView;

	// No need destroy.
	void destroy(VkDevice _device, VkImage _image, value_type _handle)
	{
		vkDestroyImageView(_device, _handle, nullptr);
	}

	VkFormat	format;
};


template<>
class	vkref<VkImageView>
		: public vkref_impl<VkImageView, VkDevice, VkImage>
{
	using	super_type	= vkref_impl<VkImageView, VkDevice, VkImage>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }


	static vkref<VkImageView> create(vkref<VkImage> const& _image)
	{
		VkImageViewCreateInfo	ci
		{
			.sType				= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext				= nullptr,
			.flags				= 0,
			.image				= _image,
			.viewType			= VK_IMAGE_VIEW_TYPE_2D,
			.format				= _image.format(),
			.components			= VkComponentMapping
			{
				.r				= VK_COMPONENT_SWIZZLE_R,
				.g				= VK_COMPONENT_SWIZZLE_G,
				.b				= VK_COMPONENT_SWIZZLE_B,
				.a				= VK_COMPONENT_SWIZZLE_A,
			},
			.subresourceRange	= VkImageSubresourceRange
			{
				.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel	= 0,
				.levelCount		= 1,
				.baseArrayLayer	= 0,
				.layerCount		= 1,
			},
		};
		return	create(_image, ci);
	}

	static vkref<VkImageView> create(vkref<VkImage> const& _image, VkImageViewCreateInfo& _ci)
	{
		auto		dev		= std::get<VkDevice>(_image);
		value_type	handle	= VK_NULL_HANDLE;
		_AMTRS_VKINVOKE(vkCreateImageView(dev, &_ci, NULL, &handle));
		return	vkref<VkImageView>(handle, dev, _image);
	}

	VkExtent2D size() const noexcept
	{
		return	std::get<VkImage>(*this).size();
	}
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
