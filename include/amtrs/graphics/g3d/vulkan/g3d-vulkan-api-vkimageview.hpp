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
		VkImageViewCreateInfo	ci{0};
		ci.sType				= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ci.pNext				= nullptr;
		ci.flags				= 0;
		ci.image				= _image;
		ci.viewType				= VK_IMAGE_VIEW_TYPE_2D;
		ci.format				= _image.format();
		ci.components.r			= VK_COMPONENT_SWIZZLE_R;
		ci.components.g			= VK_COMPONENT_SWIZZLE_G;
		ci.components.b			= VK_COMPONENT_SWIZZLE_B;
		ci.components.a			= VK_COMPONENT_SWIZZLE_A;
		ci.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		ci.subresourceRange.baseMipLevel	= 0;
		ci.subresourceRange.levelCount		= 1;
		ci.subresourceRange.baseArrayLayer	= 0;
		ci.subresourceRange.layerCount		= 1;
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
