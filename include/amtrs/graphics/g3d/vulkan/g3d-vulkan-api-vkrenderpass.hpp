﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vkrenderpass__hpp
#define	__libamtrs__g3d__vulkan__vkrenderpass__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



template<>
struct	vkhandle_traits<VkRenderPass>
{
	using	value_type	= VkRenderPass;

	// No need destroy.
	void destroy(VkDevice _device, value_type _handle)
	{
		vkDestroyRenderPass(_device, _handle, nullptr);
	}

	vkarray<VkAttachmentDescription>	attachments;
};


template<>
class	vkref<VkRenderPass>
		: public vkref_impl<VkRenderPass, VkDevice>
{
	using	super_type	= vkref_impl<VkRenderPass, VkDevice>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }


	bool is_compatible(vkref<VkRenderPass> const& _compati) const noexcept
	{
		if (_compati.handle() == handle())
		{
			return	true;
		}
		if (traits().attachments.size() != _compati.traits().attachments.size())
		{
			return	false;
		}
		for (auto i = 0; i < traits().attachments.size(); ++i)
		{
			if (traits().attachments[i].format != _compati.traits().attachments[i].format)
			{
				return	false;
			}
		}
		return	true;
	}

	// ==============================================================
	//! 指定するフォーマットにレンダリングを行うためのパスを作成します。
	// --------------------------------------------------------------
	static vkref<VkRenderPass> create(vkref<VkDevice> const& _device, VkAttachmentDescription const* _attachments, size_t _attachmentCount)
	{
		if (vklasterr) { return {}; }

		VkAttachmentReference	colorRef{0};
		colorRef.attachment	= 0;
		colorRef.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference	depthRef;
		depthRef.attachment	= 1;
		depthRef.layout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass;
		subpass.flags						= 0;
		subpass.pipelineBindPoint			= VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.inputAttachmentCount		= 0;
		subpass.pInputAttachments			= nullptr;
		subpass.colorAttachmentCount		= 1;
		subpass.pColorAttachments			= &colorRef;
		subpass.pResolveAttachments			= nullptr;
		subpass.pDepthStencilAttachment		= _attachmentCount == 2 ? &depthRef : nullptr;
		subpass.preserveAttachmentCount		= 0;
		subpass.pPreserveAttachments		= nullptr;

		VkSubpassDependency	depenency[2];
		depenency[0].srcSubpass			= VK_SUBPASS_EXTERNAL;
		depenency[0].dstSubpass			= 0;
		depenency[0].srcStageMask		= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		depenency[0].dstStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		depenency[0].srcAccessMask		= VK_ACCESS_MEMORY_READ_BIT;
		depenency[0].dstAccessMask		= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		depenency[0].dependencyFlags	= VK_DEPENDENCY_BY_REGION_BIT;

		depenency[1].srcSubpass			= 0;
		depenency[1].dstSubpass			= VK_SUBPASS_EXTERNAL;
		depenency[1].srcStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		depenency[1].dstStageMask		= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		depenency[1].srcAccessMask		= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		depenency[1].dstAccessMask		= VK_ACCESS_MEMORY_READ_BIT;
		depenency[1].dependencyFlags	= VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo ci{0};
		ci.sType					= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		ci.attachmentCount			= static_cast<uint32_t>(_attachmentCount);
		ci.pAttachments				= _attachments;
		ci.subpassCount				= 1;
		ci.pSubpasses				= &subpass;
		ci.dependencyCount			= 0;
		ci.pDependencies			= nullptr;
//		ci.dependencyCount			= 2;
//		ci.pDependencies			= depenency;

		VkRenderPass	handle	= VK_NULL_HANDLE;
		_AMTRS_VKINVOKE(vkCreateRenderPass(_device, &ci, nullptr, &handle));

		vkref			retval(handle, _device);
		retval.traits().attachments	= vkarray<VkAttachmentDescription>(_attachmentCount);
		std::copy_n(_attachments, _attachmentCount, retval.traits().attachments.data());
		return	retval;
	}



	vkarray<VkAttachmentDescription> const& attachments() const noexcept
	{
		return	traits().attachments;
	}
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
