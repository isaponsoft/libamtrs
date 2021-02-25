/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkcommandpool__hpp
#define	__libamtrs__g3d__vulkan__vkcommandpool__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



template<>
struct	vkhandle_traits<VkCommandPool>
{
	using	value_type	= VkCommandPool;

	// No need destroy.
	void destroy(VkDevice _device, value_type _handle)
	{
		vkDestroyCommandPool(_device, _handle, nullptr);
	}
};


template<>
class	vkref<VkCommandPool>
		: public vkref_impl<VkCommandPool, VkDevice>
{
	using	super_type	= vkref_impl<VkCommandPool, VkDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }


	// ==============================================================
	//! 指定するフォーマットにレンダリングを行うためのパスを作成します。
	// --------------------------------------------------------------
	static vkref<VkCommandPool> create(vkref<VkDevice> const& _device)
	{
		if (vklasterr) { return {}; }
		VkCommandPoolCreateInfo	ci{};
		ci.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		ci.pNext				= nullptr;
		ci.queueFamilyIndex		= _device.family_index();
		ci.flags				= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VkCommandPool	handle	= VK_NULL_HANDLE;
		_AMTRS_VKINVOKE(vkCreateCommandPool(_device, &ci, NULL, &handle));
		return	super_type::_make(handle, _device);
	}
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
