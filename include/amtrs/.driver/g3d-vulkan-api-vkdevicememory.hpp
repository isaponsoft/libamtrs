/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkdevicememory__hpp
#define	__libamtrs__g3d__vulkan__vkdevicememory__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


template<>
struct	vkhandle_traits<VkDeviceMemory>
{
	using	value_type	= VkDeviceMemory;
	using	size_type	= VkDeviceSize;

	// No need destroy.
	void destroy(VkDevice _device, value_type _handle)
	{
		vkFreeMemory(_device, _handle, nullptr);
	}

	size_type	size;
};


template<>
class	vkref<VkDeviceMemory>
		: public vkref_impl<VkDeviceMemory, VkDevice>
{
	using	super_type	= vkref_impl<VkDeviceMemory, VkDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }

	using	size_type	= traits_type::size_type;

	static vkref<VkDeviceMemory> allocate(vkref<VkDevice> const& _device, VkDeviceSize _bufferSize, uint32_t _typeIndex)
	{
		vkref		retval;
		value_type	handle	= VK_NULL_HANDLE;

		VkMemoryAllocateInfo	mai{};
		mai.sType			= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mai.memoryTypeIndex	= _typeIndex;
		mai.allocationSize	= _bufferSize;
		_AMTRS_VKINVOKE(vkAllocateMemory(_device, &mai, NULL, &handle));
		retval	= vkref(handle, _device);
		retval.traits().size	= _bufferSize;
		return	retval;
	}

	static vkref<VkDeviceMemory> allocate(vkref<VkDevice> const& _device, VkMemoryRequirements const& _memreq, VkFlags _properties)
	{
		auto	gpu	= std::get<VkPhysicalDevice>(_device);
		return	allocate(
					_device,
					_memreq.size,
					gpu.memory_type(_memreq.memoryTypeBits, _properties)
				);
	}

	size_type size() const noexcept { return traits().size; }

	VkResult transfer(void const* _buff, VkDeviceSize _size, VkMemoryMapFlags _flags = 0) const
	{
		auto	device	= std::get<VkDevice>(*this);
		void*	ptr		= nullptr;
		_AMTRS_VKINVOKE_R(vkMapMemory(device, get(), 0, _size, _flags, &ptr), vklasterr);
		std::memcpy(ptr, _buff, _size);
		vkUnmapMemory(device, get());
		return	vklasterr;
	}

	template<class Mapper>
	VkResult map(VkDeviceSize _offset, VkDeviceSize _size, Mapper&& _mapper, VkMemoryMapFlags _flags = 0) const
	{
		auto	device	= std::get<VkDevice>(*this);
		void*	ptr		= nullptr;
		_AMTRS_VKINVOKE_R(vkMapMemory(device, get(), _offset, _size, _flags, &ptr), vklasterr);
		_mapper(ptr, _size);
		vkUnmapMemory(device, get());
		return	vklasterr;
	}

	template<class Mapper>
	VkResult map(Mapper&& _mapper, VkMemoryMapFlags _flags = 0) const
	{
		return	map(0, size(), std::forward<Mapper>(_mapper), _flags);
	}
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
