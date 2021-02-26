/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkbuffer__hpp
#define	__libamtrs__g3d__vulkan__vkbuffer__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


template<>
struct	vkhandle_traits<VkBuffer>
{
	using	value_type	= VkBuffer;
	using	size_type	= VkDeviceSize;

	// No need destroy.
	void destroy(VkDevice _device, value_type _handle)
	{
		vkDestroyBuffer(_device, _handle, nullptr);
	}

	VkMemoryRequirements	requirements;
	size_type				size;
	vkref<VkDeviceMemory>	memory;
};


template<>
class	vkref<VkBuffer>
		: public vkref_impl<VkBuffer, VkDevice>
{
	using	super_type	= vkref_impl<VkBuffer, VkDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }

	using	size_type	= traits_type::size_type;

	static constexpr uint32_t	nidx	= UINT32_MAX;

	vkref<VkDeviceMemory> const& device_memory() const
	{
		return	traits().memory;
	}

	VkMemoryRequirements const& requirements()
	{
		return	traits().requirements;
	}

	size_type size() const noexcept { return traits().size; }

	void bind(vkref<VkDeviceMemory> const& _memory, VkDeviceSize _offset = 0)
	{
		if (vklasterr) { return; }
		auto&	t(traits());
		vklasterr	= vkBindBufferMemory(std::get<VkDevice>(*this), get(), _memory, _offset);
		if (vklasterr.good())
		{
			t.memory	= _memory;
		}
		else
		{
			AMTRS_WARN_LOG("vkBindBufferMemory : %s", std::to_string(vklasterr).c_str());\
		}
	}

	template<class Mapper>
	void allocate(VkFlags _properties, Mapper&& _mapper)
	{
		if (vklasterr) { return; }
		auto	device	= std::get<VkDevice>(*this);
		auto	memory	= vkref<VkDeviceMemory>::allocate(device, requirements(), _properties);
		if (vklasterr.good())
		{
			memory.map(std::forward<Mapper>(_mapper));
			bind(memory);
		}
	}

	void allocate(VkFlags _properties)
	{
		if (vklasterr) { return; }
		auto	device	= std::get<VkDevice>(*this);
		auto	memory	= vkref<VkDeviceMemory>::allocate(device, requirements(), _properties);
		if (vklasterr.good())
		{
			bind(memory);
		}
	}

	// ==============================================================
	//! 頂点バッファを作成します。
	// --------------------------------------------------------------
	static vkref<VkBuffer> create_vertex_buffer(vkref<VkDevice> const& _device, VkDeviceSize _bufferSize)
	{
		vkref				retval;
		VkBuffer			handle	= VK_NULL_HANDLE;
		uint32_t			family	= _device.family_index();
		VkBufferCreateInfo	ci
		{
			.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext					= nullptr,
			.flags					= 0,
			.size					= _bufferSize,
			.usage					= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			.sharingMode			= VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount	= 1,
			.pQueueFamilyIndices	= &family,
		};
		vklasterr	= vkCreateBuffer(_device, &ci, nullptr, &handle);
		if (vklasterr)
		{
			AMTRS_WARN_LOG("vkCreateBuffer: %s", std::to_string(vklasterr).c_str());
			return	{};
		}
		retval	= super_type::_make(handle, _device);

		auto&	t(retval.traits());
		vkGetBufferMemoryRequirements(_device, handle, &t.requirements);
		t.size	= _bufferSize;
		return	retval;
	}


	// ==============================================================
	//! 頂点バッファを作成します。
	// --------------------------------------------------------------
	static vkref<VkBuffer> create_vertex_buffer(vkref<VkDevice> const& _device, void const* _data, VkDeviceSize _bufferSize)
	{
		vkref<VkBuffer>	retval	= create_vertex_buffer(_device, _bufferSize);
		if (retval)
		{
			retval.allocate(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			retval.device_memory().map([&](auto data, auto size)
			{
				memcpy(data, _data, _bufferSize);
			});
		}
		return	retval;
	}

	// ==============================================================
	//! インデックスバッファを作成します。
	// --------------------------------------------------------------
	static vkref<VkBuffer> create_index_buffer(vkref<VkDevice> const& _device, VkDeviceSize _bufferSize)
	{
		vkref				retval;
		VkBuffer			handle	= VK_NULL_HANDLE;
		VkBufferCreateInfo	ci
		{
			.sType			= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size			= _bufferSize,
			.usage			= VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			.sharingMode	= VK_SHARING_MODE_EXCLUSIVE,
		};

		vklasterr	= vkCreateBuffer(_device, &ci, nullptr, &handle);
		if (vklasterr)
		{
			AMTRS_WARN_LOG("vkCreateBuffer: %s", std::to_string(vklasterr).c_str());
			return	{};
		}
		retval	= super_type::_make(handle, _device);

		auto&	t(retval.traits());
		vkGetBufferMemoryRequirements(_device, handle, &t.requirements);
		t.size	= _bufferSize;
		return	retval;
	}

	template<class SourceCopy>
	static vkref<VkBuffer> create_transfer_source(vkref<VkDevice> const& _device, VkDeviceSize _size, SourceCopy&& _src)
	{
		if (vklasterr) { return {}; }
		auto	retval	= vkref<VkBuffer>::create(_device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, _size);
		if (vklasterr)
		{
			return	{};
		}
		retval.allocate(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, std::forward<SourceCopy>(_src));
		if (vklasterr)
		{
			return	{};
		}
		return	retval;
	}

	// ==============================================================
	//! 使用方法を指定してバッファを作成します。
	// --------------------------------------------------------------
	static vkref<VkBuffer> create(vkref<VkDevice> const& _device, VkBufferUsageFlags _usage, VkDeviceSize _bufferSize)
	{
		vkref				retval;
		VkBuffer			handle	= VK_NULL_HANDLE;
		VkBufferCreateInfo	ci{};
		ci.sType	= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		ci.size		= _bufferSize;
		ci.usage	= _usage;
		ci.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
		_AMTRS_VKINVOKE(vkCreateBuffer(_device, &ci, nullptr, &handle));

		retval	= super_type::_make(handle, _device);

		auto&	t(retval.traits());
		vkGetBufferMemoryRequirements(_device, handle, &t.requirements);
		t.size	= _bufferSize;
		return	retval;
	}

	// ==============================================================
	//! UNIFORMバッファを作成します。
	// --------------------------------------------------------------
	static vkref<VkBuffer> create_uniform_buffer(vkref<VkDevice> const& _device, VkDeviceSize _bufferSize)
	{
		vkref				retval;
		VkBuffer			handle	= VK_NULL_HANDLE;
		uint32_t			family	= _device.family_index();
		VkBufferCreateInfo	ci
		{
			.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext					= nullptr,
			.flags					= 0,
			.size					= _bufferSize,
			.usage					= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			.sharingMode			= VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount	= 1,
			.pQueueFamilyIndices	= &family,
		};
		_AMTRS_VKINVOKE(vkCreateBuffer(_device, &ci, nullptr, &handle));

		retval	= super_type::_make(handle, _device);
		auto&	t(retval.traits());
		vkGetBufferMemoryRequirements(_device, handle, &t.requirements);
		t.size	= _bufferSize;
		return	retval;
	}
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
