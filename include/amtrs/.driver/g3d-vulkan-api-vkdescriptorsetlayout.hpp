/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkdescriptorsetlayout__hpp
#define	__libamtrs__g3d__vulkan__vkdescriptorsetlayout__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



template<>
struct	vkhandle_traits<VkDescriptorSetLayout>
{
	using	value_type	= VkDescriptorSetLayout;

	// No need destroy.
	void destroy(VkDevice _device, value_type _handle)
	{
		vkDestroyDescriptorSetLayout(_device, _handle, nullptr);
	}

	vkarray<VkDescriptorSetLayoutBinding>	bindings;
};


template<>
class	vkref<VkDescriptorSetLayout>
		: public vkref_impl<VkDescriptorSetLayout, VkDevice>
{
	using	super_type	= vkref_impl<VkDescriptorSetLayout, VkDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }

	vkarray<VkDescriptorSetLayoutBinding> const& bindings() const noexcept
	{
		return	traits().bindings;
	}

	//! CreateInfo を指定して作成する
	static vkref<VkDescriptorSetLayout> create(vkref<VkDevice> const& _device, VkDescriptorSetLayoutCreateInfo const& _ci)
	{
		if (vklasterr) { return {}; }
		vkref		retval;
		value_type	handle	= VK_NULL_HANDLE;
		vklasterr	= vkCreateDescriptorSetLayout(_device, &_ci, nullptr, &handle);
		if (vklasterr)
		{
			AMTRS_WARN_LOG("vkCreateDescriptorSetLayout: %s", std::to_string(vklasterr).c_str());
			return	{};
		}
		retval	= super_type::_make(handle, _device);
		retval.traits().bindings	= vkarray<VkDescriptorSetLayoutBinding>(_ci.bindingCount);
		std::copy_n(_ci.pBindings, _ci.bindingCount, retval.traits().bindings.data());
		return	retval;
	}

	//! VkDescriptorSetLayoutBinding の配列を指定して作成する
	static vkref<VkDescriptorSetLayout> create(vkref<VkDevice> const& _device, vkarray<VkDescriptorSetLayoutBinding> const& _bindings)
	{
		if (vklasterr) { return {}; }
		VkDescriptorSetLayoutCreateInfo		ci
		{
			.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount	= static_cast<uint32_t>(_bindings.size()),
			.pBindings		= _bindings.data(),
		};
		return	create(_device, ci);
	}

	//! シェーダーステージとデスクリプタタイプを指定して作成する簡易ラッパー
	struct	binding_params
	{
		VkShaderStageFlags	stage;
		VkDescriptorType	decType;
	};
	static vkref<VkDescriptorSetLayout> create_with_stages(vkref<VkDevice> const& _device, vkarray<binding_params> const& _bindings)
	{
		if (vklasterr) { return {}; }
		vkarray<VkDescriptorSetLayoutBinding>	binding(_bindings.size());
		for (int i = 0; i < binding.size(); ++i)
		{
			binding[i]	= {
				.binding			= 0,
				.descriptorType		= _bindings[i].decType,
				.descriptorCount	= 1,
				.stageFlags			= _bindings[i].stage,
				.pImmutableSamplers	= nullptr,
			};
		}
		return	create(_device, binding);
	}
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
