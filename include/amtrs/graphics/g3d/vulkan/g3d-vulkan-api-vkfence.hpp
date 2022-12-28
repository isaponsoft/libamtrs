/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkfence__hpp
#define	__libamtrs__g3d__vulkan__vkfence__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



template<>
struct	vkhandle_traits<VkFence>
{
	using	value_type	= VkFence;

	// No need destroy.
	void destroy(VkDevice _device, value_type _handle)
	{
		vkDestroyFence(_device, _handle, nullptr);
	}
};


template<>
class	vkref<VkFence>
		: public vkref_impl<VkFence, VkDevice>
{
	using	super_type	= vkref_impl<VkFence, VkDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }

	template<class Execute>
	VkResult execute(Execute&& _execute, uint64_t _wait = UINT64_MAX) const noexcept
	{
		_execute(*this);
		VkFence	f;
		vklasterr	= vkWaitForFences(std::get<VkDevice>(*this), 1, &f, VK_FALSE, _wait);
		return	vklasterr;
	}

	VkResult reset() const noexcept
	{
		VkFence	f(get());
		vklasterr	= vkResetFences(std::get<VkDevice>(*this), 1, &f);
		return	vklasterr;
	}

	VkResult wait(uint64_t _wait = UINT64_MAX)
	{
		VkFence	f	= get();
		vklasterr	= vkWaitForFences(std::get<VkDevice>(*this), 1, &f, VK_TRUE, _wait);
		return	vklasterr;
	}

	// ==============================================================
	//! 指定するウィンドウコンテキストのサーフェースを生成します。
	// --------------------------------------------------------------
	static vkref<VkFence> create(vkref<VkDevice> const& _device)
	{
		if (vklasterr) { return {}; }

		VkFenceCreateInfo	ci{};
		ci.sType		= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		VkFence	handle	= VK_NULL_HANDLE;
		vklasterr	= vkCreateFence(_device, &ci, nullptr, &handle);
		return	super_type::_make(handle, _device);
	}
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
