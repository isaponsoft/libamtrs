/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vksampler__hpp
#define	__libamtrs__g3d__vulkan__vksampler__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


template<>
struct	vkhandle_traits<VkSampler>
{
	using	value_type	= VkSampler;

	// No need destroy.
	void destroy(VkDevice _device, value_type _handle)
	{
		vkDestroySampler(_device, _handle, nullptr);
	}
};


template<>
class	vkref<VkSampler>
		: public vkref_impl<VkSampler, VkDevice>
{
	using	super_type	= vkref_impl<VkSampler, VkDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }


	static vkref<VkSampler> create(vkref<VkDevice> const& _device)
	{
		VkSamplerCreateInfo	ci
		{
			.sType						= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.pNext						= nullptr,
			.flags						= 0,
			.magFilter					= VK_FILTER_LINEAR,
			.minFilter					= VK_FILTER_LINEAR,
			.mipmapMode					= VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU				= VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeV				= VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeW				= VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.mipLodBias					= 0.0f,
			.anisotropyEnable			= VK_FALSE,
			.compareEnable				= VK_FALSE,
			.compareOp 					= VK_COMPARE_OP_NEVER,
			.minLod						= 0.0f,
			.maxLod						= 0.0f,
			.borderColor				= VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
			.unnormalizedCoordinates	= VK_FALSE,
		};

#if	0
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(std::get<VkPhysicalDevice>(_device), &properties);
		ci.anisotropyEnable	= VK_TRUE;
		ci.maxAnisotropy	= properties.limits.maxSamplerAnisotropy;
#else
		ci.anisotropyEnable	= VK_FALSE;
		ci.maxAnisotropy	= 1.0f;
#endif

		VkSampler			handle	= VK_NULL_HANDLE;
		_AMTRS_VKINVOKE(vkCreateSampler(_device, &ci, nullptr, &handle));
		return	vkref(handle, _device);
	}
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
