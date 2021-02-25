/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vksemaphore__hpp
#define	__libamtrs__g3d__vulkan__vksemaphore__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



template<>
struct	vkhandle_traits<VkSemaphore>
{
	using	value_type	= VkSemaphore;

	// No need destroy.
	void destroy(VkDevice _device, value_type _handle)
	{
		vkDestroySemaphore(_device, _handle, nullptr);
	}
};


template<>
class	vkref<VkSemaphore>
		: public vkref_impl<VkSemaphore, VkDevice>
{
	using	super_type	= vkref_impl<VkSemaphore, VkDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }


	// ==============================================================
	//! 指定するウィンドウコンテキストのサーフェースを生成します。
	// --------------------------------------------------------------
	static vkref<VkSemaphore> create(vkref<VkDevice> const& _device)
	{
		if (vklasterr) { return {}; }

		VkSemaphoreCreateInfo	ci{};
		ci.sType		= VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		value_type	handle	= VK_NULL_HANDLE;
		_AMTRS_VKINVOKE(vkCreateSemaphore(_device, &ci, nullptr, &handle));
		return	super_type::_make(handle, _device);
	}
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
