/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkimage__hpp
#define	__libamtrs__g3d__vulkan__vkimage__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


template<>
struct	vkhandle_traits<VkImage>
{
	using	value_type	= VkImage;

	// No need destroy.
	void destroy(VkDevice _device, value_type _handle)
	{
		if (!no_destroy)
		{
			vkDestroyImage(_device, _handle, nullptr);
		}
	}


	VkFormat				format;
	VkExtent2D				size;
	bool					no_destroy	= false;
	vkref<VkDeviceMemory>	mem;
};


template<>
class	vkref<VkImage>
		: public vkref_impl<VkImage, VkDevice>
{
	using	super_type	= vkref_impl<VkImage, VkDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }


	// ==============================================================
	//! 破棄しないImageを生成します。
	// --------------------------------------------------------------
	//! SwapChain などから取得したイメージなどに対して使用します。
	// --------------------------------------------------------------
	static vkref<VkImage> create_no_destroy(VkImage _image, vkref<VkDevice> const& _device, VkExtent2D _size, VkFormat _format)
	{
		vkref<VkImage>	retval(_image, _device);
		retval.traits().format		= _format;
		retval.traits().size		= _size;
		retval.traits().no_destroy	= true;
		return	retval;
	}

	static vkref<VkImage> create(vkref<VkDevice> const& _device, VkExtent2D _size, VkFormat _format = VK_FORMAT_R8G8B8A8_UNORM)
	{
		VkImageCreateInfo	ci{0};
		ci.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		ci.pNext			= nullptr;
		ci.flags			= 0;
		ci.imageType		= VK_IMAGE_TYPE_2D;
		ci.format			= VK_FORMAT_R8G8B8A8_SRGB;
		ci.extent.width		= _size.width;
		ci.extent.height	= _size.height;
		ci.extent.depth		= 1;
		ci.mipLevels		= 1;
		ci.arrayLayers		= 1;
		ci.samples			= VK_SAMPLE_COUNT_1_BIT;
		ci.tiling			= VK_IMAGE_TILING_OPTIMAL;
		ci.usage			= VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		ci.sharingMode		= VK_SHARING_MODE_EXCLUSIVE;
		ci.initialLayout	= VK_IMAGE_LAYOUT_PREINITIALIZED;	//VK_IMAGE_LAYOUT_UNDEFINED;
		return	create(_device, ci);
	}


	static vkref<VkImage> create(vkref<VkDevice> const& _device, VkImageCreateInfo& _createinfo)
	{
		VkImage			handle	= VK_NULL_HANDLE;
		vkref<VkImage>	retval;
		_AMTRS_VKINVOKE(vkCreateImage(_device, &_createinfo, nullptr, &handle));

		retval	= vkref<VkImage>(super_type::_make(handle, _device));
		retval.traits().format	= _createinfo.format;
		retval.traits().size	= {_createinfo.extent.width, _createinfo.extent.height};

		return	retval;
	}

	VkFormat format() const noexcept
	{
		return	traits().format;
	}

	VkExtent2D size() const noexcept
	{
		return	traits().size;
	}

	VkMemoryRequirements requirements()
	{
		VkMemoryRequirements	retval;
		vkGetImageMemoryRequirements(std::get<VkDevice>(*this), get(), &retval);
		return	retval;
	}
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
