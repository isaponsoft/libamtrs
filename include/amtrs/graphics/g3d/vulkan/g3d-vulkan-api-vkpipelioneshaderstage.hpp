/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkpipelineshaderstage__hpp
#define	__libamtrs__g3d__vulkan__vkpipelineshaderstage__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


template<>
struct	vkhandle_traits<VkPipelineShaderStage>
{
	using	value_type	= VkPipelineShaderStage;

	// No need destroy.
	void destroy(VkDevice _device, value_type _handle)
	{
		vkDestroyShaderModule(_device, _handle, nullptr);
	}
};


template<>
class	vkref<VkPipelineShaderStage>
		: public vkref_impl<VkPipelineShaderStage, VkDevice>
{
	using	super_type	= vkref_impl<VkPipelineShaderStage, VkDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }



	// ==============================================================
	// --------------------------------------------------------------
	static vkref<VkShaderModule> create(vkref<VkDevice> const& _device, void const* _code, size_t _codeSize)
	{
		VkShaderModule 				handle	= VK_NULL_HANDLE;
		VkShaderModuleCreateInfo	ci{};
		ci.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		ci.codeSize		= _codeSize;
		ci.pCode		= reinterpret_cast<const uint32_t*>(_code);
		_AMTRS_VKINVOKE(vkCreateShaderModule(_device, &ci, nullptr, &handle));
		return	super_type::_make(handle, _device);
	}

	// ==============================================================
	// --------------------------------------------------------------
	template<class CodeT>
	static vkref<VkShaderModule> create(vkref<VkDevice> const& _device, CodeT const& _code)
	{
		VkShaderModule 				handle	= VK_NULL_HANDLE;
		VkShaderModuleCreateInfo	ci{};
		ci.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		ci.codeSize		= _code.size();
		ci.pCode		= reinterpret_cast<const uint32_t*>(_code.data());
		_AMTRS_VKINVOKE(vkCreateShaderModule(_device, &createInfo, nullptr, &handle));
		return	super_type::_make(handle, _device);
	}
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
