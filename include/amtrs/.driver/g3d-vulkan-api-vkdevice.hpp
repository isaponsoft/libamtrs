/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkdevice__hpp
#define	__libamtrs__g3d__vulkan__vkdevice__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



template<>
struct	vkhandle_traits<VkDevice>
{
	using	value_type	= VkDevice;

	// No need destroy.
	void destroy(VkPhysicalDevice _gpu, value_type _handle)
	{
		vkDestroyDevice(_handle, nullptr);
	}


	uint32_t				familyIndex;
	VkQueueFamilyProperties	prop;
};


template<>
class	vkref<VkDevice>
		: public vkref_impl<VkDevice, VkPhysicalDevice>
{
	using	super_type	= vkref_impl<VkDevice, VkPhysicalDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }

	using	famly_index_type	= vkref<VkPhysicalDevice>::famly_index_type;


	// ==============================================================
	//! 指定するGPUのキューを示すデバイスを作成します。。
	// --------------------------------------------------------------
	static vkref<VkDevice> create(vkref<VkPhysicalDevice> const& _gpu, famly_index_type _queueFamilyIndex, std::initializer_list<char const*> _extensions)
	{
		std::vector<const char *>		extensionNames(_extensions);

		VkDeviceQueueCreateInfo	dqci{};
		float					qp[]	= {0.0f};
		dqci.sType					= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		dqci.pNext					= nullptr;
		dqci.queueFamilyIndex 		= _queueFamilyIndex;
		dqci.queueCount				= 1;
		dqci.pQueuePriorities 		= qp;

		VkDeviceCreateInfo		di{};
		di.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		di.pNext					= nullptr;
		di.queueCreateInfoCount		= 1;
		di.pQueueCreateInfos		= &dqci;
		di.enabledExtensionCount	= static_cast<uint32_t>(extensionNames.size());
		di.ppEnabledExtensionNames	= extensionNames.data();
		di.pEnabledFeatures			= nullptr;

		VkDevice		handle	= VK_NULL_HANDLE;
		vklasterr	= vkCreateDevice(_gpu, &di, NULL, &handle);
		vkref<VkDevice>	retval(super_type::_make(handle, _gpu));
		if (vklasterr.good())
		{
			retval.initialize(_queueFamilyIndex);
		}
		return	retval;
	}

	famly_index_type family_index() const noexcept { return traits().familyIndex; }

	// createの中から呼ばれます
	void initialize(famly_index_type _queueFamily)
	{
		auto&	t(traits());
		t.familyIndex	= _queueFamily;
		t.prop			= std::get<VkPhysicalDevice>(*this).get_queue_familry()[_queueFamily];
	}
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
