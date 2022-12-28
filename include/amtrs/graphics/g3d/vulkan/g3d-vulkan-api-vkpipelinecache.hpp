/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkpipelinecache__hpp
#define	__libamtrs__g3d__vulkan__vkpipelinecache__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



template<>
struct	vkhandle_traits<VkPipelineCache>
{
	using	value_type	= VkPipelineCache;

	// No need destroy.
	void destroy(VkDevice _device, value_type _handle)
	{
		vkDestroyPipelineCache(_device, _handle, nullptr);
	}
};


template<>
class	vkref<VkPipelineCache>
		: public vkref_impl<VkPipelineCache, VkDevice>
{
public:
	using	super_type	= vkref_impl<VkPipelineCache, VkDevice>;
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }


	static vkref<VkPipelineCache> create(vkref<VkDevice> const& _device)
	{
		if (vklasterr) { return {}; }

		VkPipelineCacheCreateInfo	ci{0};
		ci.sType			= VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		ci.pNext			= nullptr;
		ci.flags			= 0;
		ci.initialDataSize	= 0;
		ci.pInitialData		= nullptr;
		vkref		retval;
		value_type	handle	= VK_NULL_HANDLE;
		_AMTRS_VKINVOKE(vkCreatePipelineCache(_device, &ci, nullptr, &handle));
		retval	= super_type::_make(handle, _device);
		return	retval;
	}
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
