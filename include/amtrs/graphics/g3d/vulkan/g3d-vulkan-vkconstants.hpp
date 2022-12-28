/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vkconstants__hpp
#define	__libamtrs__driver__g3d__vulkan__vkconstants__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



class	vkconstants
		: public ref_object
{
public:
	static ref<vkconstants> realize(constants& _uniform, vkref<VkDevice> const& _device)
	{
		return	realizing<vkconstants>(_uniform, _device);
	}

	void initialize(constants::instance_type* _owner, vkref<VkDevice> const& _device)
	{
		mOwner		= _owner;

		auto	format	= _owner->layout;
		auto	layout	= vkpipelinelayout::realize(format, _device)->mSetLayout;
		mBuffer	= vkref<VkBuffer>::create_uniform_buffer(_device, format.stride());
		mBuffer.allocate(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}

	void update(vkref<VkCommandBuffer> const& _cmdbuff, void const* _data, size_t _size)
	{
		vkCmdUpdateBuffer(_cmdbuff, mBuffer, 0, _size, _data);
	}

	constants::instance_type*		mOwner;
	vkref<VkBuffer>					mBuffer;
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
