/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vkvertexbuffer__hpp
#define	__libamtrs__driver__g3d__vulkan__vkvertexbuffer__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


class	vkvertexbuffer
		: public ref_object
{
public:
	void initialize(vertexbuffer::instance_type* _owner, vkref<VkDevice> const& _device, void const* _data = nullptr, size_t _size = 0)
	{
		mOwner		= _owner;
AMTRS_WARN_LOG("Vertics %zd", _size);
		mVertexBuff	= vkref<VkBuffer>::create_vertex_buffer(_device, _data, _size);
	}


	vertexbuffer::instance_type*	mOwner;
	vkref<VkBuffer>					mVertexBuff;
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
