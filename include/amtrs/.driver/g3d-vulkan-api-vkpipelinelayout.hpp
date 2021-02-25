/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkpipelinelayout__hpp
#define	__libamtrs__g3d__vulkan__vkpipelinelayout__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



template<>
struct	vkhandle_traits<VkPipelineLayout>
{
	using	value_type	= VkPipelineLayout;

	// No need destroy.
	void destroy(VkDevice _device, value_type _handle)
	{
		vkDestroyPipelineLayout(_device, _handle, nullptr);
	}
};


template<>
class	vkref<VkPipelineLayout>
		: public vkref_impl<VkPipelineLayout, VkDevice>
{
	using	super_type	= vkref_impl<VkPipelineLayout, VkDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }


	static vkref<VkPipelineLayout> create(vkref<VkDescriptorSetLayout> const& _layout)
	{
		if (vklasterr) { return {}; }

		auto					device	= std::get<VkDevice>(_layout);
		VkDescriptorSetLayout	layout	= _layout;

		VkPipelineLayoutCreateInfo		ci{};
		ci.sType			= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		ci.setLayoutCount	= 1;
		ci.pSetLayouts		= &layout;

		value_type	handle	= VK_NULL_HANDLE;
		_AMTRS_VKINVOKE(vkCreatePipelineLayout(device, &ci, nullptr, &handle));
		return	super_type::_make(handle, device);
	}

	static vkref<VkPipelineLayout> create(vkref<VkDevice> const& _device)
	{
		if (vklasterr) { return {}; }

		VkPipelineLayoutCreateInfo		ci{};
		ci.sType			= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		ci.setLayoutCount	= 0;
		ci.pSetLayouts		= nullptr;

		value_type	handle	= VK_NULL_HANDLE;
		_AMTRS_VKINVOKE(vkCreatePipelineLayout(_device, &ci, nullptr, &handle));
		return	super_type::_make(handle, _device);
	}
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
