/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vkmesh__hpp
#define	__libamtrs__driver__g3d__vulkan__vkmesh__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



//! Vulkan用のメッシュ
//! 最低でもひとつ以上の頂点バッファからなる。
class	vkmesh
		: public ref_object
{
public:

	void bind_and_draw(VkCommandBuffer _cmdbuff)
	{
		bind(_cmdbuff);
		draw(_cmdbuff);
	}


	void bind(VkCommandBuffer _cmdbuff)
	{
		VkDeviceSize	offset		= 0;
		VkBuffer		buffers		= mVertexBuff;
		vkCmdBindVertexBuffers(_cmdbuff, 0, 1, &buffers, &offset);
	}


	void draw(VkCommandBuffer _cmdbuff)
	{
		vkCmdDraw(_cmdbuff, mVertexCount, 1, 0, 0);
	}


	void create_vertex_buffer(vkref<VkDevice> const& _device, size_t _stride, void const* _buffer, size_t _buffersize)
	{
		mVertexStride	= static_cast<uint32_t>(_stride);
		mVertexCount	= static_cast<uint32_t>(_buffersize / mVertexStride);
		mVertexBuff		= vkref<VkBuffer>::create_vertex_buffer(_device, _buffer, _buffersize);
	}

	template<class T>
	void create_vertex_buffer(vkref<VkDevice> const& _device, void const* _buffer, size_t _buffersize)
	{
		create_vertex_buffer(_device, sizeof(T), _buffer, _buffersize);
	}

	uint32_t		mVertexStride;
	uint32_t		mVertexCount;
	vkref<VkBuffer>	mVertexBuff;
//	vkref<VkBuffer>	indexbuff;
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
