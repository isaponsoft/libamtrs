/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkdescriptorset__hpp
#define	__libamtrs__g3d__vulkan__vkdescriptorset__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



template<>
struct	vkhandle_traits<VkDescriptorSet>
{
	using	value_type	= VkDescriptorSet;

	void destroy(vkref<VkDescriptorPool> const& _pool, VkDescriptorSetLayout _layout, value_type _handle)
	{
		vkFreeDescriptorSets(std::get<VkDevice>(_pool), _pool, 1, &_handle);
	}
};



template<>
class	vkref<VkDescriptorSet>
		: public vkref_impl<VkDescriptorSet, VkDescriptorPool, VkDescriptorSetLayout>
{
	using	super_type	= vkref_impl<VkDescriptorSet, VkDescriptorPool, VkDescriptorSetLayout>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }

	static vkref<VkDescriptorSet> allocate(vkref<VkDescriptorPool> const& _pool, vkref<VkDescriptorSetLayout> const& _layout)
	{
		if (vklasterr) { return {}; }
		VkDescriptorSetLayout	layouts[1]	= { _layout };
		VkDescriptorSetAllocateInfo	ai
		{
			.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext				= nullptr,
			.descriptorPool		= _pool,
			.descriptorSetCount	= 1,
			.pSetLayouts 		= layouts,
		};
		value_type	handle	= VK_NULL_HANDLE;
		_AMTRS_VKINVOKE(vkAllocateDescriptorSets(std::get<VkDevice>(_pool), &ai, &handle));
		return	super_type::_make(handle, _pool, _layout);
	}

	//! レイアウトをもとに DescriptorSet を指定した数だけ用意します。
	static vkarray<vkref<VkDescriptorSet>> create(vkref<VkDescriptorSetLayout> const& _layoutset, uint32_t _size)
	{
		if (vklasterr) { return {}; }
		vkarray<vkref<VkDescriptorSet>>	retval(_size);
		vkref<VkDescriptorPool>			pool	= vkref<VkDescriptorPool>::create(_layoutset, _size);
		if (vklasterr)
		{
			return	{};
		}
		for (uint32_t i = 0; i < _size; ++i)
		{
			retval[i]	= allocate(pool, _layoutset);
			if (vklasterr) { return {}; }
		}
		return	retval;
	}
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
