/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vkframebuffer__hpp
#define	__libamtrs__driver__g3d__vulkan__vkframebuffer__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


class	vkframebuffer
		: public ref_object
{
public:
	static ref<vkframebuffer> realize(framebuffer& _frmbuff, vkref<VkDevice> const& _device)
	{
		return	realizing<vkframebuffer>(_frmbuff, _device);
	}

	void initialize(framebuffer::instance_type* _owner,  vkref<VkDevice> const& _device)
	{
		mOwner		= _owner;
		mDevice		= _device;
	}

	vkref<VkFramebuffer>			mFramebuff;
	vkref<VkRenderPass>				mRenderpass;
	std::vector<vkref<VkImageView>>	mViews;
	framebuffer::instance_type*		mOwner;
	vkref<VkDevice>					mDevice;
	VkFormat						mFormat;


	vkref<VkFramebuffer> framebuff() const noexcept
	{
		return	mFramebuff;
	}

	std::vector<vkref<VkImageView>> const& views() const noexcept
	{
		return	mViews;
	}

	vkref<VkRenderPass> renderpass() const noexcept
	{
		return	mRenderpass;
	}

	//! _renderpass
	//!		RenderPassBegin で行うレンダリングパスと互換性のあるレンダリングパス。
	//!		フレームバッファを作成するために一時的に使用するものなので
	//!		破棄してもかまわない。
	void build(vkref<VkDevice> const& _device, vkref<VkSwapchainKHR> const& _swapchain, int _index)
	{
		std::vector<vkref<VkImageView>>	images;
		images.reserve(3);

		// カラーバッファ
		vkref<VkImageView>	colview	= _swapchain.image_view(_index);
		vkref<VkImage>		colimg	= std::get<VkImage>(colview);
		images.push_back(colview);
		
		// 深度バッファの確認
		if (!mOwner->depth.empty())
		{
			auto	depth	= mOwner->depth;
			auto	tx		= vktexture::realize(depth, size2<int>(colimg.size().width, colimg.size().height), _device, nullptr, 0);
			VkImageViewCreateInfo	ci{0};
			ci.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			ci.pNext							= nullptr;
			ci.flags							= 0;
			ci.image							= tx->mImage;
			ci.viewType							= VK_IMAGE_VIEW_TYPE_2D;
			ci.format							= tx->mImage.format();
			ci.components.r						= VK_COMPONENT_SWIZZLE_R;
			ci.components.g						= VK_COMPONENT_SWIZZLE_G;
			ci.components.b						= VK_COMPONENT_SWIZZLE_B;
			ci.components.a						= VK_COMPONENT_SWIZZLE_A;
			ci.subresourceRange.aspectMask		= has_stencil(tx->mImage.format())
												? VkImageAspectFlags(VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT)
												: VK_IMAGE_ASPECT_DEPTH_BIT;
			ci.subresourceRange.baseMipLevel	= 0;
			ci.subresourceRange.levelCount		= 1;
			ci.subresourceRange.baseArrayLayer	= 0;
			ci.subresourceRange.layerCount		= 1;
			images.push_back(vkref<VkImageView>::create(tx->mImage, ci));
		}

		// レンダリングパスを生成
		VkAttachmentDescription	attachments[2]{0};

		// color buffer
		attachments[0].format					= colimg.format();
		attachments[0].samples					= VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp					= VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp					= VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp			= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout			= VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout				= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// depth buffer
		if (images.size() >= 2)
		{
			attachments[1].format					= std::get<VkImage>(images[1]).format();
			attachments[1].samples					= VK_SAMPLE_COUNT_1_BIT;
			attachments[1].loadOp					= VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[1].storeOp					= VK_ATTACHMENT_STORE_OP_STORE;
			attachments[1].stencilLoadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[1].stencilStoreOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[1].initialLayout			= VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[1].finalLayout				= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
		mRenderpass	= vkref<VkRenderPass>::create(_device, attachments, images.size());
		mFramebuff	= vkref<VkFramebuffer>::create(colimg.size(), mRenderpass, images.data(), images.size());
		mViews		= std::move(images);
	}
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
