/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vksurface__hpp
#define	__libamtrs__g3d__vulkan__vksurface__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN

template<class...>
struct	surface_creator;


template<>
struct	vkhandle_traits<VkSurfaceKHR>
{
	using	value_type	= VkSurfaceKHR;

	// No need destroy.
	void destroy(VkInstance _instance, value_type _handle)
	{
		if (!physical_surface)
		{
			vkDestroySurfaceKHR(_instance, _handle, nullptr);
		}
	}

	bool	physical_surface	= false;
};


template<>
class	vkref<VkSurfaceKHR>
		: public vkref_impl<VkSurfaceKHR, VkInstance>
{
	using	super_type	= vkref_impl<VkSurfaceKHR, VkInstance>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }

	// ==============================================================
	//! 指定するウィンドウコンテキストのサーフェースを生成します。
	// --------------------------------------------------------------
	template<class... Args>
	static vkref<VkSurfaceKHR> create(vkref<VkInstance> const& _instance, Args const&... _createArgs)
	{
		if (vklasterr) { return {}; }
		VkSurfaceKHR	handle	= VK_NULL_HANDLE;
		vklasterr	= surface_creator<Args...>{}.create(&handle, _instance, _createArgs...);
		if (vklasterr)
		{
			return	{};
		}
		vkref<VkSurfaceKHR>	retval	= super_type::_make(handle, _instance);
		retval.traits().physical_surface	= true;
		return	retval;
	}


	// ============================================================================
	//! GPUに対するフォーマットを取得します。
	// ----------------------------------------------------------------------------
	VkFormat format(VkPhysicalDevice _gpu) const noexcept
	{
		if (vklasterr) { return VK_FORMAT_UNDEFINED; }
		auto	formats	= vkenum<VkSurfaceFormatKHR>([&](auto _count, auto _buff) {
							return	vkGetPhysicalDeviceSurfaceFormatsKHR(_gpu, get(), _count, _buff);
						});
		if (formats.empty())
		{
			// おそらく VkInstance に VK_KHR_SURFACE_EXTENSION_NAME がセットされていない
			vklasterr	= VK_ERROR_INITIALIZATION_FAILED;
			return	VK_FORMAT_UNDEFINED;
		}

		VkFormat	fmt	= formats.front().format;
		return	(fmt == VK_FORMAT_UNDEFINED && formats.size() == 1)
				? VK_FORMAT_B8G8R8A8_UNORM
				: fmt;
	}


	// ============================================================================
	//! GPUに対するフォーマットを列挙し、コールバックが返した最大値のものを返します。
	// ----------------------------------------------------------------------------
	template<class Compare>
	VkSurfaceFormatKHR choise_format(VkPhysicalDevice _gpu, Compare _compare) const noexcept
	{
		if (vklasterr) { return {VK_FORMAT_UNDEFINED,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }; }
		auto	formats	= vkenum<VkSurfaceFormatKHR>([&](auto _count, auto _buff) {
							return	vkGetPhysicalDeviceSurfaceFormatsKHR(_gpu, get(), _count, _buff);
						});
		int					level	= -1;
		VkSurfaceFormatKHR	retval	= formats[0];
		for (auto fmt : formats)
		{
			int	l	= _compare(fmt);
			if (l > level)
			{
				level	= l;
				retval	= fmt;
			}
		}
		return	retval;
	}

	// ============================================================================
	//! GPUから見たキャパシティを返します。
	// ----------------------------------------------------------------------------
	VkSurfaceCapabilitiesKHR caps(VkPhysicalDevice _gpu) const noexcept
	{
		if (vklasterr) { return {}; }
		VkSurfaceCapabilitiesKHR	retval;
		vklasterr	= vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_gpu, this->get(), &retval);
		return	retval;
	}


	// ============================================================================
	//! サーフェースのサイズを返します。
	// ----------------------------------------------------------------------------
	VkExtent2D size(VkPhysicalDevice _gpu) const noexcept
	{
		return	caps(_gpu).currentExtent;
	}
};


#ifdef	VK_USE_PLATFORM_ANDROID_KHR
template<>
struct	surface_creator<ANativeWindow*>
{
	VkResult create(VkSurfaceKHR* _result, VkInstance _instance, ANativeWindow* _window)
	{
		VkAndroidSurfaceCreateInfoKHR	cio{
		  .sType	= VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
		  .pNext	= nullptr,
		  .flags	= 0,
		  .window	= _window
		};
		return	vkCreateAndroidSurfaceKHR(_instance, &cio, nullptr, _result);
	}
};
#endif


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
