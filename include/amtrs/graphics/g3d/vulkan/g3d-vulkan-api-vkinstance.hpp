/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkinstance__hpp
#define	__libamtrs__g3d__vulkan__vkinstance__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


template<>
struct	vkhandle_traits<VkInstance>
{
	using	value_type	= VkInstance;

	// No need destroy.
	void destroy(value_type _handle)
	{
		vkDestroyInstance(_handle, nullptr);
	}
};




template<>
class	vkref<VkInstance>
		: public vkref_impl<VkInstance>
{
	using	super_type	= vkref_impl<VkInstance>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator value_type;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }


	static vkref create(char const* _appname, std::initializer_list<char const*> _extensions, uint32_t _apiVersion = VK_API_VERSION_1_0)
	{
		std::vector<char const*>	extensions;
		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		for (auto e : _extensions)
		{
			extensions.push_back(e);
		}
		return	create(_appname, std::move(extensions), {}, _apiVersion);
	}

	static vkref create(char const* _appname, std::initializer_list<char const*> _extensions, std::initializer_list<char const*> _layers, uint32_t _apiVersion = VK_API_VERSION_1_0)
	{
		std::vector<char const*>	extensions(_extensions);
		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		for (auto e : _extensions)
		{
			extensions.push_back(e);
		}
		return	create(_appname, std::move(extensions), std::move(_layers), _apiVersion);
	}

#if	0
	// layer
	"VK_EXT_validation_features"
#endif

	// ============================================================================
	//! インスタンスを生成します。
	// ----------------------------------------------------------------------------
	static vkref create(char const* _appname, std::vector<char const*> _extensions, std::vector<const char *> _layer, uint32_t _apiVersion = VK_API_VERSION_1_0)
	{
		if (!vklasterr.good())
		{
			return	{};
		}

		VkValidationFeatureEnableEXT	enables[] = {
			VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
		//	VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
		//	VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT
		};
		VkValidationFeaturesEXT			features{0};
		features.sType							= VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		features.pNext							= nullptr;
		features.enabledValidationFeatureCount	= static_cast<uint32_t>(sizeof(enables) / sizeof(enables[0]));
		features.pEnabledValidationFeatures		= enables;


		VkApplicationInfo	app{};
		app.sType					= VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app.pApplicationName		= _appname;
		app.apiVersion				= _apiVersion;

		VkInstanceCreateInfo	ci{};
		ci.sType					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		ci.pNext					= &features;
		ci.pApplicationInfo			= &app;
		ci.enabledLayerCount		= (uint32_t)_layer.size();
		ci.ppEnabledLayerNames		= (const char *const *)_layer.data();
		ci.enabledExtensionCount	= (uint32_t)_extensions.size();
		ci.ppEnabledExtensionNames	= (const char *const *)_extensions.data();

		VkInstance	handle	= VK_NULL_HANDLE;
		_AMTRS_VKINVOKE(vkCreateInstance(&ci, nullptr, &handle));
		return	vkref(handle);
	}




	// ============================================================================
	//! 物理デバイスを列挙します。
	// ----------------------------------------------------------------------------
	//! Callback
	//!		bool(VkPhysicalDevice>)
	//!		true を返すとコールバックを終了します。
	//!
	//! return
	//!		コールバックが true を返した場合は true を返したときの PhysicalDevice
	//!		一度も false を返さなかった場合は empty() を返します。
	// ----------------------------------------------------------------------------
	template<class Callback>
	VkPhysicalDevice enumerate_physicaldevice(Callback&& _callback) const
	{
		auto	devices	= vkenum<VkPhysicalDevice>([&](uint32_t* _count, auto _buff) {
							return	vkEnumeratePhysicalDevices(get(), _count, _buff);
						});
		if (vklasterr)
		{
			return	VK_NULL_HANDLE;
		}
		for (auto device : devices)
		{
			if (_callback(device))
			{
				return	device;
			}
		}
		return	VK_NULL_HANDLE;
	}
};





AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
