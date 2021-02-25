﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vktexture__hpp
#define	__libamtrs__driver__g3d__vulkan__vktexture__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


class	vktexture
		: public ref_object
{
public:
	//! swapchain::get_imagebuffer() から呼ばれる。
	static ref<vktexture> realize(textureset& _texture, vkref<VkSwapchainKHR> const& _swapchain, int _index)
	{
		return	realizing<vktexture>(_texture, _swapchain, _index);
	}


	static ref<vktexture> realize(textureset& _texture, size2<int> _colsize, vkref<VkDevice> const& _device, void const* _data, size_t _size)
	{
		return	realizing<vktexture>(_texture, _colsize, _device, _data, _size);
	}



	static ref<vktexture> realize(textureset& _texture, vkref<VkDevice> const& _device)
	{
		return	realizing<vktexture>(_texture, _device);
	}

	void initialize(textureset::instance_type* _owner, vkref<VkDevice> const& _device)
	{
		mOwner	= _owner;
		VkImageCreateInfo	ci
		{
			.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext			= nullptr,
			.flags			= 0,
			.imageType		= VK_IMAGE_TYPE_2D,
			.format			= VK_FORMAT_R8G8B8A8_UNORM,
			.extent			= VkExtent3D
			{
				.width	= mOwner->size.width,
				.height	= mOwner->size.height,
				.depth	= 1,
			},
			.mipLevels		= 1,
			.arrayLayers	= 1,
			.samples		= VK_SAMPLE_COUNT_1_BIT,
			.tiling			= VK_IMAGE_TILING_OPTIMAL,
			.usage			= VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.sharingMode	= VK_SHARING_MODE_EXCLUSIVE,
			.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED,//VK_IMAGE_LAYOUT_PREINITIALIZED,	//VK_IMAGE_LAYOUT_UNDEFINED,
		};

		ref<vktexture>		thiz	= new vktexture();
		vkref<VkImage>		img		= vkref<VkImage>::create(_device, ci);
		mImage				= img;
		auto				memreq	= img.requirements();
		auto				mem		= vkref<VkDeviceMemory>::allocate(_device, memreq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vkBindImageMemory(_device, img, mem, 0);
		mDevMem				= mem;
		mStagingModified	= true;
	}

	void initialize(textureset::instance_type* _owner, vkref<VkSwapchainKHR> const& _swapchain, int _index)
	{
		mOwner		= _owner;
		mImageView	= _swapchain.image_view(_index);
	}

	void initialize(textureset::instance_type* _owner, size2<int> _colsize, vkref<VkDevice> const& _device, void const* _data, size_t _size)
	{
		mOwner		= _owner;
		VkImageCreateInfo	ci
		{
			.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext			= nullptr,
			.flags			= 0,
			.imageType		= VK_IMAGE_TYPE_2D,
			.format			= VK_FORMAT_R8G8B8A8_UNORM,
			.extent			= VkExtent3D
			{
				.width	= mOwner->size.width,
				.height	= mOwner->size.height,
				.depth	= 1,
			},
			.mipLevels		= 1,
			.arrayLayers	= 1,
			.samples		= VK_SAMPLE_COUNT_1_BIT,
			.tiling			= VK_IMAGE_TILING_OPTIMAL,
			.usage			= VK_IMAGE_USAGE_SAMPLED_BIT,
			.sharingMode	= VK_SHARING_MODE_EXCLUSIVE,
			.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED,//VK_IMAGE_LAYOUT_PREINITIALIZED,	//VK_IMAGE_LAYOUT_UNDEFINED,
		};
		if (ci.extent.width == 0 || ci.extent.height == 0)
		{
			ci.extent.width		= _colsize.width;
			ci.extent.height	= _colsize.height;
		}
		if (_data)
		{
			ci.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		ref<vktexture>		thiz	= new vktexture();
		if (mOwner->usage & (textureset::usage_depth|textureset::usage_stencil))
		{
			auto					gpu	= std::get<VkPhysicalDevice>(_device);
			std::vector<VkFormat>	formats;
			if (mOwner->usage & (textureset::usage_depth|textureset::usage_stencil))
			{
				formats	= std::vector<VkFormat>({VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT});
			}
			else
			{
				formats	= std::vector<VkFormat>({VK_FORMAT_D16_UNORM, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT});
			}
			auto	fmt	= gpu.find_supported(ci.tiling, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, formats);
			ci.format	= fmt;
			ci.usage	= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}
		vkref<VkImage>		img		= vkref<VkImage>::create(_device, ci);
		mImage		= img;
		auto				memreq	= img.requirements();
		auto				mem		= vkref<VkDeviceMemory>::allocate(_device, memreq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vkBindImageMemory(_device, img, mem, 0);
		mDevMem	= mem;

		if (_data)
		{
			// テクスチャ転送用のステージングバッファ
			mStagingBuffer		= vkref<VkBuffer>::create_transfer_source(_device, _size, [&](auto dest, auto size)
			{
				std::memcpy(dest, _data, std::min<size_t>((size_t)_size, (size_t)size));
			});
		}

		VkImageViewCreateInfo	vci
		{
			.sType				= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext				= nullptr,
			.flags				= 0,
			.image				= img,
			.viewType			= VK_IMAGE_VIEW_TYPE_2D,
			.format				= img.format(),
			.components			= VkComponentMapping
			{
				.r				= VK_COMPONENT_SWIZZLE_R,
				.g				= VK_COMPONENT_SWIZZLE_G,
				.b				= VK_COMPONENT_SWIZZLE_B,
				.a				= VK_COMPONENT_SWIZZLE_A,
			},
			.subresourceRange	= VkImageSubresourceRange
			{
				.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel	= 0,
				.levelCount		= 1,
				.baseArrayLayer	= 0,
				.layerCount		= 1,
			},
		};
		if (mOwner->usage & (textureset::usage_depth|textureset::usage_stencil))
		{
		}
		else
		{
			mImageView	= vkref<VkImageView>::create(img, vci);
		}
		mStagingModified	= true;
	}


	void sync(vkref<VkCommandBuffer> _cmd, ref<vkgraphics> _graphics)
	{
		if (!mStagingModified)
		{
			return;
		}

		mStagingModified	= false;

	//	auto				cb	= vkref<VkCommandBuffer>::create(vkref<VkCommandPool>::create(_graphics->device()));
	//	cb.begin();

		auto				cb	= _cmd;

		vkref<VkImage>		img	= mImage;

		VkImageMemoryBarrier	imb
		{
			.sType					= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext					= nullptr,
			.srcAccessMask			= 0,
			.dstAccessMask			= VK_ACCESS_TRANSFER_WRITE_BIT,
			.oldLayout				= VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout				= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.srcQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED,
			.image					= img,
			.subresourceRange =
			{
				.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel	= 0,
				.levelCount		= 1,
				.baseArrayLayer	= 0,
				.layerCount		= 1,
			},
		};

		vkCmdPipelineBarrier(
			cb,
			VK_PIPELINE_STAGE_HOST_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &imb);

		VkBufferImageCopy	bic
		{
			.bufferOffset		= 0,
			.bufferRowLength	= 0,		// require 0
			.bufferImageHeight	= 0,		// require 0
			.imageSubresource	= VkImageSubresourceLayers
			{
				.aspectMask 	= VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel		= 0,
				.baseArrayLayer	= 0,
				.layerCount 	= 1,
			},
			.imageOffset		= { 0, 0, 0 },
			.imageExtent		= VkExtent3D
			{
				.width			= img.size().width,
				.height			= img.size().height,
				.depth			= 1,
			}
		};
		vkCmdCopyBufferToImage(cb, mStagingBuffer, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bic);

		imb.srcAccessMask	= VK_ACCESS_TRANSFER_WRITE_BIT;
		imb.dstAccessMask	= VK_ACCESS_SHADER_READ_BIT;
		imb.oldLayout		= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imb.newLayout		= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		vkCmdPipelineBarrier(
			cb,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &imb);

	//	cb.end();
	//	_graphics->submit(cb);
	//	vkQueueWaitIdle(_graphics->queue());

		// 画像を更新したのでViewもアップデート
		mImageView			= vkref<VkImageView>::create(img);
	}

	textureset::instance_type*	mOwner;
	vkref<VkImage>				mImage;
	vkref<VkImageView>			mImageView;
	vkref<VkDeviceMemory>		mDevMem;
	vkref<VkBuffer>				mStagingBuffer;
	bool						mStagingModified	= false;
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
