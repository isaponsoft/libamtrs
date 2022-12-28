/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vkrenderstate__hpp
#define	__libamtrs__driver__g3d__vulkan__vkrenderstate__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


class	vkrenderstate
		: public ref_object
{
public:
	//! 変更が無い項目の実体化
	void initialize(renderstate::instance_type* _renderstate)
	{
		mOwner	= _renderstate;

		renderstate::status const&	s	= *mOwner;

		// カリング
		rasterization.cullMode	= s.culling == renderstate::cull_mode_back	? VK_CULL_MODE_BACK_BIT
								: s.culling == renderstate::cull_mode_front	? VK_CULL_MODE_FRONT_BIT
								: VK_CULL_MODE_NONE;
	
		// 深度バッファ
		depthStencil.depthTestEnable	= s.depth.enable	? VK_TRUE : VK_FALSE;
		depthStencil.depthWriteEnable	= s.depth.write		? VK_TRUE : VK_FALSE;
		depthStencil.depthCompareOp		= to_vkval(s.depth.compare);

		// ステンシル
		depthStencil.stencilTestEnable	= s.stencil.enable	? VK_TRUE : VK_FALSE;
		depthStencil.front.compareMask	= s.stencil.compareMask;
		depthStencil.front.failOp		= to_vkval(s.stencil.stencil_fail);
		depthStencil.front.passOp		= to_vkval(s.stencil.depth_pass);
		depthStencil.front.depthFailOp	= to_vkval(s.stencil.depth_fail);
		depthStencil.front.writeMask	= s.stencil.writeMask;
		depthStencil.front.reference	= s.stencil.reference;
		depthStencil.front.compareOp	= to_vkval(s.stencil.compare);
		depthStencil.back				= depthStencil.front;
	}


	VkPipelineRasterizationStateCreateInfo	rasterization
	{
		/*.sType						= */VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		/*.pNext						= */nullptr,
		/*.flags						= */0,
		/*.depthClampEnable				= */VK_FALSE,
		/*.rasterizerDiscardEnable		= */VK_FALSE,
		/*.polygonMode					= */VK_POLYGON_MODE_FILL,
		/*.cullMode						= */VK_CULL_MODE_NONE,		// VK_CULL_MODE_BACK_BIT
		/*.frontFace					= */VK_FRONT_FACE_CLOCKWISE,
		/*// depth                      */
		/*.depthBiasEnable				= */VK_FALSE,
		/*.depthBiasConstantFactor		= */0,
		/*.depthBiasClamp				= */0,
		/*.depthBiasSlopeFactor			= */0,
		/*//                            */
		/*.lineWidth					= */1,
	};

	VkPipelineColorBlendAttachmentState		colorBlendAttachment[1]
	{
		{
			/*.blendEnable			= */VK_TRUE,
			/*.srcColorBlendFactor	= */VK_BLEND_FACTOR_SRC_ALPHA,
			/*.dstColorBlendFactor	= */VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			/*.colorBlendOp			= */VK_BLEND_OP_ADD,
			/*.srcAlphaBlendFactor	= */VK_BLEND_FACTOR_ONE,
			/*.dstAlphaBlendFactor	= */VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
			/*.alphaBlendOp			= */VK_BLEND_OP_ADD,
			/*.colorWriteMask		= */VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		}
	};

	VkPipelineColorBlendStateCreateInfo		colorBlendState
	{
		/*.sType			= */VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		/*.pNext			= */nullptr,
		/*.flags			= */0,
		/*.logicOpEnable	= */VK_FALSE,
		/*.logicOp 			= */VK_LOGIC_OP_COPY,
		/*.attachmentCount	= */1,
		/*.pAttachments		= */colorBlendAttachment,
		/*.blendConstants	= */{0,0,0,0,},
	}; 

	VkPipelineDepthStencilStateCreateInfo	depthStencil
	{
		/*.sType					= */VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		/*.pNext					= */NULL,
		/*.flags					= */0,
		/*.depthTestEnable		= */VK_TRUE,
		/*.depthWriteEnable		= */VK_TRUE,
		/*.depthCompareOp			= */VK_COMPARE_OP_LESS_OR_EQUAL,
		/*.depthBoundsTestEnable	= */VK_FALSE,
		/*.stencilTestEnable		= */VK_FALSE,
		//.front					= */VkStencilOpState 
		{
			/*.failOp			= */VK_STENCIL_OP_REPLACE,
			/*.passOp			= */VK_STENCIL_OP_REPLACE,
			/*.depthFailOp		= */VK_STENCIL_OP_REPLACE,
			/*.compareOp		= */VK_COMPARE_OP_ALWAYS,
			/*.compareMask		= */0,
			/*.writeMask		= */0,
			/*.reference		= */0,
		},
		//.back					= VkStencilOpState 
		{
			/*.failOp			= */VK_STENCIL_OP_REPLACE,
			/*.passOp			= */VK_STENCIL_OP_REPLACE,
			/*.depthFailOp		= */VK_STENCIL_OP_REPLACE,
			/*.compareOp		= */VK_COMPARE_OP_ALWAYS,
			/*.compareMask		= */0,
			/*.writeMask		= */0,
			/*.reference		= */0,
		},
		/*.minDepthBounds		= */0,
		/*.maxDepthBounds		= */1,
	};

private:
	renderstate::instance_type*				mOwner;
};

AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
