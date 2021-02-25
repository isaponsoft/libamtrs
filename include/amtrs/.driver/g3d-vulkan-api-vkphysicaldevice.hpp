/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkphysicaldevice__hpp
#define	__libamtrs__g3d__vulkan__vkphysicaldevice__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


template<>
struct	vkhandle_traits<VkPhysicalDevice>
{
	using	value_type			= VkPhysicalDevice;
	using	famly_index_type	= uint32_t;

	// No need destroy.
	void destroy(VkInstance _instance, value_type _handle)
	{
	}

	struct	queue_family
	{
		famly_index_type		index;
		VkQueueFamilyProperties	properties;
	};
	vkarray<queue_family>				family;
	VkPhysicalDeviceMemoryProperties	memPropeties;
};


template<>
class	vkref<VkPhysicalDevice>
		: public vkref_impl<VkPhysicalDevice, VkInstance>
{
	using	super_type	= vkref_impl<VkPhysicalDevice, VkInstance>;
	using	traits_type	= typename super_type::traits_type;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }

	using	famly_index_type			= typename traits_type::famly_index_type;

	static constexpr uint32_t	nidx	= UINT32_MAX;


	// ============================================================================
	//! 物理デバイスを列挙します。
	// ----------------------------------------------------------------------------
	//! Callback
	//!		bool(vkref<VkPhysicalDevice>)
	//!		true を返すとコールバックを終了します。
	//!
	//! return
	//!		コールバックが true を返した場合は true を返したときの PhysicalDevice
	//!		一度も false を返さなかった場合は empty() を返します。
	// ----------------------------------------------------------------------------
	template<class Callback>
	static vkref<VkPhysicalDevice> enumerate(vkref<VkInstance> const& _instance, Callback&& _callback)
	{
		auto	devices	= vkenum<VkPhysicalDevice>([&](uint32_t* _count, auto _buff)
						{
							return	vkEnumeratePhysicalDevices(_instance, _count, _buff);
						});
		if (vklasterr)
		{
			return	{};
		}
		int	index	= 0;
		for (auto device : devices)
		{
			vkref<VkPhysicalDevice>	dev	= vkref<VkPhysicalDevice>::create(_instance, index);
			if (_callback(dev))
			{
				return	dev;
			}
			++index;
		}
		return	{};
	}



	// ============================================================================
	//! 指定するサーフェースとキューをサポートするGPUを返します。
	// ----------------------------------------------------------------------------
	static vkref<VkPhysicalDevice> create(vkref<VkInstance> const& _instance, VkSurfaceKHR _surface, unsigned int _queueFlags = VK_QUEUE_GRAPHICS_BIT)
	{
		return	enumerate(_instance, [&](auto gpu)
				{
					auto	familyIndex	= gpu.find_family(_surface, _queueFlags);
					if (familyIndex != nidx)
					{
						return	true;
					}
					return	false;
				});
	}

	// ============================================================================
	//! 指定するサーフェースとキューをサポートするファミリを返します。
	// ----------------------------------------------------------------------------
	famly_index_type find_family(VkSurfaceKHR _surface, unsigned int _queueFlags)
	{
		famly_index_type	familyIndex	= 0;
		for (auto prop : get_queue_familry())
		{
			if ((prop.queueFlags & _queueFlags) == _queueFlags)
			{
				if (support(familyIndex, _surface))
				{
					return	familyIndex;
				}
			}
			++familyIndex;
		}
		return	nidx;
	}

	// ============================================================================
	//! 指定する番号の物理デバイスを返します。
	// ----------------------------------------------------------------------------
	static vkref<VkPhysicalDevice> create(vkref<VkInstance> const& _instance, uint32_t _index = 0)
	{
		auto	gpu	= 	_instance.enumerate_physicaldevice([&](auto dev)
					{
						if (_index == 0)
						{
							return	true;
						}
						--_index;
						return	false;
					});
		if (!gpu)
		{
			return	{};
		}
		vkref<VkPhysicalDevice>	retval(gpu, _instance);
		if (!retval.empty())
		{
			auto&		t(retval.traits());
			auto		fmly(retval.get_queue_familry());
			t.family	= vkarray<traits_type::queue_family>(fmly.size());
			for (famly_index_type index = 0; index < fmly.size(); ++index)
			{
				t.family[index].index		= index;
				t.family[index].properties	= fmly[index];
			}

			vkGetPhysicalDeviceMemoryProperties(retval.get(), &t.memPropeties);
		}
		return	retval;
	}


	// ==============================================================
	//! present queue と graphics queue を返す。
	// --------------------------------------------------------------
	std::pair<uint32_t, uint32_t> get_queue_families(VkSurfaceKHR _surface) const
	{
		uint32_t	present		= find_present_queue_family(_surface);
		uint32_t	graphics	= present;
		if (present == nidx)
		{
			// Graphicsをサポートするプレゼントが見つからなかったので別々に用意
			present		= find_present_queue_family(_surface, 0);
			graphics	= find_graphics_queue_family();
		}
		return	{present, graphics};
	}



	// ==============================================================
	//! 指定するキューファミリがサーフェースをサポートするか調べます。
	// --------------------------------------------------------------
	bool support(famly_index_type _familyIndex, VkSurfaceKHR _surface) const noexcept
	{
		if (vklasterr.good())
		{
			VkBool32	s;
			vklasterr = vkGetPhysicalDeviceSurfaceSupportKHR(get(), _familyIndex, _surface, &s);
			if (vklasterr.good())
			{
				return	s ? true : false;
			}
		}
		return	false;
	}


	// ==============================================================
	//! 指定したマスクと一致するファミリをひとつ返します。
	// --------------------------------------------------------------
	//!	_queueFlags
	//!		マスクと一致するファミリを返します。
	//!		VkQueueFlagBits の組み合わせで指定するか 0 を指定します。
	//!		省略時は VK_QUEUE_GRAPHICS_BIT です。
	//! _count
	//!		何番目に見つかったファミリなのかを指定します。
	// --------------------------------------------------------------
	famly_index_type find_queue_family(unsigned int _queueFlags, int _count = 0) const noexcept
	{
		for (auto& f : traits().family)
		{
			if ((f.properties.queueFlags & _queueFlags) == _queueFlags)
			{
				if (_count == 0)
				{
					return	f.index;
				}
				--_count;
			}
		}
		return	nidx;
	}


	// ==============================================================
	//! グラフィックスファミリをひとつ返します。
	// --------------------------------------------------------------
	famly_index_type find_graphics_queue_family(int _count = 0) const noexcept
	{
		return	find_queue_family(VK_QUEUE_GRAPHICS_BIT, _count);
	}


	// ==============================================================
	//! 指定するサーフェースをサポートするキューファミリを取得します。
	// --------------------------------------------------------------
	//! _queueFlags
	//!		マスクと一致するファミリを返します。
	//!		VkQueueFlagBits の組み合わせで指定するか 0 を指定します。
	//!		省略時は VK_QUEUE_GRAPHICS_BIT です。
	// --------------------------------------------------------------
	famly_index_type find_present_queue_family(VkSurfaceKHR _surface, unsigned int _queueFlags = VK_QUEUE_GRAPHICS_BIT) const noexcept
	{
		for (auto it = traits().family.begin(); (it != traits().family.end()) && (vklasterr.good()); ++it)
		{
			if (support(it->index, _surface))
			{
				if ((it->properties.queueFlags & _queueFlags) == _queueFlags)
				{
					return	it->index;
				}
			}
		}
		return	nidx;
	}


	// ==============================================================
	//! プロパティを返します。
	// --------------------------------------------------------------
	VkPhysicalDeviceProperties properties() const noexcept
	{
		VkPhysicalDeviceProperties	retval;
		if (!empty())
		{
			vkGetPhysicalDeviceProperties(get(), &retval);
		}
		return	retval;
	}


	// ============================================================================
	//! VkQueueFamilyPropertiesの一覧を返します。
	// ----------------------------------------------------------------------------
	vkarray<VkQueueFamilyProperties> get_queue_familry() const noexcept
	{
		return	vkenum<VkQueueFamilyProperties>([&](auto* _count, auto _buff)
						{
							vkGetPhysicalDeviceQueueFamilyProperties(get(), _count, _buff);
							return	VK_SUCCESS;
						});
	}


	VkPhysicalDeviceMemoryProperties const& memory_properties() const noexcept
	{
		return	traits().memPropeties;
	}


	// ============================================================================
	//! メモリタイプを調べます。
	// ----------------------------------------------------------------------------
	//!	return
	//!		見つからない場合は nidx を返します。
	// ----------------------------------------------------------------------------
	uint32_t memory_type(uint32_t _typeBits, VkFlags _properties)
	{
		auto&	memtypes(memory_properties().memoryTypes);
		for (uint32_t index = 0; index < memory_properties().memoryTypeCount; ++index)
		{
			if (_typeBits & 0x01)
			{
				if ((memtypes[index].propertyFlags & _properties) == _properties)
				{
					return	index;
				}
			}
			_typeBits >>= 1;
		}
		return	nidx;
	}

	// ============================================================================
	//! 指定するフォーマットに対するプロパティを得ます
	// ----------------------------------------------------------------------------
	VkFormatProperties properties(VkFormat _format)
	{
		VkFormatProperties	retval;
		vkGetPhysicalDeviceFormatProperties(get(), _format, &retval);
		return	retval;
	}


	// ============================================================================
	//! サポートするフォーマットを調べる
	// ----------------------------------------------------------------------------
	VkFormat find_supported(VkImageTiling _tiling, VkFormatFeatureFlags _features, std::vector<VkFormat> const& _formats)
	{
		for (VkFormat fmt : _formats)
		{
			auto	props	= properties(fmt);
			if (_tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & _features) == _features)
			{
				return	fmt;
			}
			if (_tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & _features) == _features)
			{
				return fmt;
			}
		}
		return	VK_FORMAT_UNDEFINED;
	}
};




AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
