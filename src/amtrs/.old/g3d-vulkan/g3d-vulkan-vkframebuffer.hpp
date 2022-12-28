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
	vkref<VkFramebuffer>			mFramebuffer;
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
		if (!mFramebuffer.empty())
		{
			return;
		}



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
			VkImageViewCreateInfo	ci
			{
				.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.pNext							= nullptr,
				.flags							= 0,
				.image							= tx->mImage,
				.viewType						= VK_IMAGE_VIEW_TYPE_2D,
				.format							= tx->mImage.format(),
				.components	= VkComponentMapping         
				{
					.r					= VK_COMPONENT_SWIZZLE_R,
					.g					= VK_COMPONENT_SWIZZLE_G,
					.b					= VK_COMPONENT_SWIZZLE_B,
					.a					= VK_COMPONENT_SWIZZLE_A,
				},
				.subresourceRange	= VkImageSubresourceRange
				{
					.aspectMask			= has_stencil(tx->mImage.format())
										? VkImageAspectFlags(VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT)
										: VK_IMAGE_ASPECT_DEPTH_BIT,
					.baseMipLevel		= 0,
					.levelCount			= 1,
					.baseArrayLayer		= 0,
					.layerCount			= 1,
				},
			};
			images.push_back(vkref<VkImageView>::create(tx->mImage, ci));
		}


		if (mRenderpass)
		{
			// レンダリングパスを作り直す必要があるかどうか
			if (mRenderpass.attachments().size() != images.size() || mRenderpass.attachments()[0].format != colimg.format())
			{
				mRenderpass.clear();
			}
		}

		// レンダリングパスを生成
		if (!mRenderpass)
		{
			VkAttachmentDescription	attachments[]
			{
				{
					.format						= colimg.format(),
					.samples					= VK_SAMPLE_COUNT_1_BIT,
					.loadOp						= VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp					= VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp				= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp				= VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout				= VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout				= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				},
				{
					.format						= std::get<VkImage>(images[1]).format(),
					.samples					= VK_SAMPLE_COUNT_1_BIT,
					.loadOp						= VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp					= VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp				= VK_ATTACHMENT_LOAD_OP_CLEAR,
					.stencilStoreOp				= VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout				= VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout				= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				},
			};
			mRenderpass	= vkref<VkRenderPass>::create(_device, attachments, images.size());
		}

		mFramebuff	= vkref<VkFramebuffer>::create(colimg.size(), mRenderpass, images.data(), images.size());
		mViews		= std::move(images);
	}
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
