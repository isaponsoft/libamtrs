/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkdebugreport__hpp
#define	__libamtrs__g3d__vulkan__vkdebugreport__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


template<>
struct	vkhandle_traits<VkDebugReportCallbackEXT>
{
	using	value_type	= VkDebugReportCallbackEXT;


	// No need destroy.
	void destroy(VkInstance _instance, value_type _handle)
	{
		auto	destroy	= (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugReportCallbackEXT");
		destroy(_instance, _handle, nullptr);
	}

	struct	callback_data : ref_object
	{
	};

	PFN_vkCreateDebugReportCallbackEXT	vkCreateDebugReportCallback	= nullptr;
	ref<callback_data>					callback;
};




template<>
class	vkref<VkDebugReportCallbackEXT>
		: public vkref_impl<VkDebugReportCallbackEXT, VkInstance>
{
	using	super_type	= vkref_impl<VkDebugReportCallbackEXT, VkInstance>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator value_type;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }

	// ============================================================================
	//! インスタンスを生成します。
	// ----------------------------------------------------------------------------
	template<class Callback>
	static vkref create(vkref<VkInstance> const& _instance, Callback&& _callback)
	{
		struct	callback_type
				: traits_type::callback_data
		{
			callback_type(Callback&& _callback)
				: callback(std::move(_callback))
			{}
			Callback	callback;
		};

		ref<callback_type>	cb	= new callback_type(std::move(_callback));

		auto	vkCallback	= (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT");

		VkDebugReportCallbackCreateInfoEXT ci{};
		ci.sType		= VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		ci.flags		= VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		ci.pUserData	= cb.get();
		ci.pfnCallback	= (PFN_vkDebugReportCallbackEXT)[](VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char* pLayerPrefix, const char* pMsg, void* pUserData) -> VkBool32 
		{
			auto*	callback	= reinterpret_cast<callback_type*>(pUserData);
			callback->callback(flags, objType, srcObject, location, msgCode, pLayerPrefix, pMsg);
			return	VK_FALSE;
		};

		value_type	handle	= VK_NULL_HANDLE;
		vkCallback(_instance, &ci, nullptr, &handle);
		vkref	retval(handle, _instance);
		retval.traits().vkCreateDebugReportCallback	= vkCallback;
		retval.traits().callback					= cb;
		return	retval;
	}



};





AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
