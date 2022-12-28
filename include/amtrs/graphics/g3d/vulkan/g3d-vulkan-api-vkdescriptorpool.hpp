/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkdescriptorpool__hpp
#define	__libamtrs__g3d__vulkan__vkdescriptorpool__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


template<>
struct	vkhandle_traits<VkDescriptorPool>
{
	using	value_type	= VkDescriptorPool;

	void destroy(VkDevice _device, value_type _handle)
	{
		vkDestroyDescriptorPool(_device, _handle, nullptr);
	}
};


template<>
class	vkref<VkDescriptorPool>
		: public vkref_impl<VkDescriptorPool, VkDevice>
{
	using	super_type	= vkref_impl<VkDescriptorPool, VkDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }


	static vkref<VkDescriptorPool> create(vkref<VkDevice> const& _device, VkDescriptorPoolCreateInfo const& _createinfo)
	{
		if (vklasterr) { return {}; }
		value_type	handle	= VK_NULL_HANDLE;
		_AMTRS_VKINVOKE(vkCreateDescriptorPool(_device, &_createinfo, nullptr, &handle));
		return	super_type::_make(handle, _device);
	}


	static vkref<VkDescriptorPool> create(vkref<VkDevice> const& _device, uint32_t _maxsets, vkarray<VkDescriptorPoolSize> const& _sizelist)
	{
		if (vklasterr) { return {}; }
		VkDescriptorPoolCreateInfo	ci{0};
		ci.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		ci.pNext			= nullptr;
		ci.flags			= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		ci.maxSets			= _maxsets;
		ci.poolSizeCount	= static_cast<uint32_t>(_sizelist.size());
		ci.pPoolSizes		= _sizelist.data();
		return	create(_device, ci);
	}


	//! bindingの情報を元にDescriptorPoolを生成する
	static vkref<VkDescriptorPool> create(vkref<VkDevice> const& _device, vkarray<VkDescriptorSetLayoutBinding> const& _bindings, uint32_t _maxsets)
	{
		if (vklasterr) { return {}; }
		vkarray<VkDescriptorPoolSize>	sizes(_bindings.size());
		for (int i = 0; i < sizes.size(); ++i)
		{
			sizes[i]	= {0};
			sizes[i].type				= _bindings[i].descriptorType;
			sizes[i].descriptorCount	= _bindings[i].descriptorCount;
		}
		return	create(_device, _maxsets, sizes);
	}

	//! DescriptorSetLayoutに設定されているbindingの情報を元にDescriptorPoolを生成する
	static vkref<VkDescriptorPool> create(vkref<VkDescriptorSetLayout> const& _layoutset, uint32_t _maxsets)
	{
		return	create(std::get<VkDevice>(_layoutset), _layoutset.bindings(), _maxsets);
	}
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
