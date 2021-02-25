/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkpipeline__hpp
#define	__libamtrs__g3d__vulkan__vkpipeline__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



template<>
struct	vkhandle_traits<VkPipeline>
{
	using	value_type	= VkPipeline;

	// No need destroy.
	void destroy(vkref<VkDevice> const& _device, value_type _handle)
	{
		vkDestroyPipeline(_device, _handle, nullptr);
	}
};


template<>
class	vkref<VkPipeline>
		: public vkref_impl<VkPipeline, VkDevice>
{
	using	super_type	= vkref_impl<VkPipeline, VkDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }


	static vkref<VkPipeline> create(vkref<VkDevice> _device, vkref<VkPipelineCache> _cache, VkGraphicsPipelineCreateInfo* _createinfo)
	{
		value_type	handle	= VK_NULL_HANDLE;
		_AMTRS_VKINVOKE(vkCreateGraphicsPipelines(_device, _cache, 1, _createinfo, nullptr, &handle));
		return	vkref(handle, std::move(_device));
	}
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
