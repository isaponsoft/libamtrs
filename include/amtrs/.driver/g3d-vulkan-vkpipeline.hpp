/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vkpipeline__hpp
#define	__libamtrs__driver__g3d__vulkan__vkpipeline__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



class	vkpipeline
		: public ref_object
{
public:
	//! 変更が無い項目の実体化
	void initialize(pipeline::instance_type* _pipeline, vkref<VkDevice> const& _device, vkref<VkPipelineCache> const& _cache)
	{
		mOwner	= _pipeline;

		// マテリアルの初期化
		material_layout	layout	= mOwner->mat.layout();
		mFormat		= vkpipelinelayout::realize(layout, _device);
		for (int i = 0; i < (int)shader_stage::_max; ++i)
		{
			if (auto sm = mOwner->mat.shader((shader_stage)i); !sm.empty())
			{
				mShaders[i]	= vkshadermodule::realize(sm, _device);
			}
		}

		mCache			= _cache;
		mLayout			= mFormat->mLayout;
		mSetLayout		= mFormat->mSetLayout;
		mShaderVertex	= shader_module(shader_stage::vertex_stage);
		mShaderFragment	= shader_module(shader_stage::fragment_stage);
		input_format(mOwner->mat.layout().inputformat().data(), mOwner->mat.layout().inputformat().size());
	}



	vkref<VkPipeline> const& pipeline(vkref<VkRenderPass> _renderpass, VkExtent2D _viewsize, renderstate* _currentstate)
	{
		// 前回の呼び出しから変わっていなければパイプラインの生成はスキップする
		if (!mRenderpass.empty() && mRenderpass.is_compatible(_renderpass))
		{
			return	mPipeline;
		}

		// パイプラインを作り直す
		mRenderpass	= _renderpass;

		vkrenderstate*	rs	= realizing<vkrenderstate>(mOwner->rs.empty() ? *_currentstate : mOwner->rs);

		// 頂点シェーダーへの入力情報
		VkPipelineVertexInputStateCreateInfo	vertex
		{
			.sType								= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext								= nullptr,
			.flags								= 0,
			.vertexBindingDescriptionCount 		= static_cast<uint32_t>(mVertexInputBinding.size()),
			.pVertexBindingDescriptions			= mVertexInputBinding.data(),
			.vertexAttributeDescriptionCount  	= static_cast<uint32_t>(mVertexInputAttributes.size()),
			.pVertexAttributeDescriptions		= mVertexInputAttributes.data(),
		};

		// 入力アセンブリ
		VkPipelineInputAssemblyStateCreateInfo	iasci
		{
			.sType					= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext					= nullptr,
			.flags					= 0,
			.topology				= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable	= VK_FALSE,
		};

		// ビューポートとシザー
		VkViewport viewports
		{
			.x			= 0,
			.y			= 0,
			.width		= (float)_viewsize.width,
			.height		= (float)_viewsize.height,
			.minDepth	= 0,
			.maxDepth	= 1,
		};;
		VkRect2D	scissor
		{
			.offset	= { 0, 0 },
			.extent	= { _viewsize.width, _viewsize.height },
		};
		VkPipelineViewportStateCreateInfo		vsci
		{
			.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext			= nullptr,
			.flags			= 0,
			.viewportCount	= 1,
			.pViewports		= &viewports,
			.scissorCount	= 1,
			.pScissors		= &scissor,
		};


		// サンプラー
		VkPipelineMultisampleStateCreateInfo	msci
		{
			.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.pNext					= nullptr,
			.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable	= VK_FALSE,
			.minSampleShading		= 0,
			.pSampleMask			= nullptr,
			.alphaToCoverageEnable	= VK_FALSE,
			.alphaToOneEnable		= VK_FALSE,
		};

		// シェーダー
		VkPipelineShaderStageCreateInfo			stage[]
		{
			mShaderVertex	.shader_stage(VK_SHADER_STAGE_VERTEX_BIT, "main"),
			mShaderFragment	.shader_stage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"),
		};

		VkGraphicsPipelineCreateInfo		ci
		{
			.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext					= nullptr,
			.flags					= 0,
			.stageCount				= 2,
			.pStages				= stage,
			.pVertexInputState		= &vertex,
			.pInputAssemblyState	= &iasci,
			.pTessellationState		= nullptr,
			.pViewportState			= &vsci,
			.pRasterizationState	= &rs->rasterization,
			.pMultisampleState		= &msci,
			.pDepthStencilState 	= &rs->depthStencil,
			.pColorBlendState		= &rs->colorBlendState,
			.pDynamicState			= nullptr,
			.layout					= mLayout,
			.renderPass				= mRenderpass,
			.subpass				= 0,
			.basePipelineHandle		= nullptr,
			.basePipelineIndex		= 0,
		};

		mRenderpass	= _renderpass;
		mPipeline	= vkref<VkPipeline>::create(std::get<VkDevice>(mCache), {}, &ci);

		return	mPipeline;
	}


	void input_format(g3d::attribute const* _attr, size_t _size)
	{
		// stride を計算
		auto	bind	= vkarray<VkVertexInputBindingDescription>(1);
		auto	attr	= vkarray<VkVertexInputAttributeDescription>(_size);


		uint32_t	offset		= 0;
		uint32_t	location	= 0;
		for (uint32_t i = 0; i < _size; ++i)
		{
			attr[i]	= 
			{
				.location	= location,
				.binding	= 0,
				.format		= (_attr[i].type == g3d::element_type::float_value && _attr[i].count == 4) ? VK_FORMAT_R32G32B32A32_SFLOAT
							: (_attr[i].type == g3d::element_type::float_value && _attr[i].count == 3) ? VK_FORMAT_R32G32B32_SFLOAT
							: (_attr[i].type == g3d::element_type::float_value && _attr[i].count == 2) ? VK_FORMAT_R32G32_SFLOAT
							: (_attr[i].type == g3d::element_type::float_value && _attr[i].count == 1) ? VK_FORMAT_R32_SFLOAT
							: VK_FORMAT_R32_SFLOAT,
				.offset		= offset,
			};
			++location;
			offset	+= static_cast<uint32_t>((_attr[i].type == g3d::element_type::float_value ? sizeof(float) : 1) * _attr[i].count);
		}
		bind[0]	= 
		{
			.binding	= 0,
			.stride		= offset,
			.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX,
		};

		mVertexInputBinding		= std::move(bind);
		mVertexInputAttributes	= std::move(attr);
	}


	vkshadermodule* shader(shader_stage _stage)
	{
		return	mShaders[(int)_stage];
	}

	vkref<VkShaderModule> shader_module(shader_stage _stage)
	{
		if (auto sm = shader(_stage); sm)
		{
			return	sm->mModule;
		}
		return	{};
	}

	vkref<VkPipelineLayout> layout()
	{
		return	mFormat->mLayout;
	}

	vkref<VkDescriptorSetLayout> descset_layout()
	{
		return	mFormat->mSetLayout;
	}


	pipeline::instance_type*					mOwner;

	vkref<VkRenderPass>							mRenderpass;
	vkref<VkPipeline>							mPipeline;		// パイプライン本体
	vkref<VkPipelineCache>						mCache;			// 関連付けているキャッシュ

	vkpipelinelayout*							mFormat;
	vkref<VkPipelineLayout>						mLayout;
	vkref<VkDescriptorSetLayout>				mSetLayout;
	vkref<VkShaderModule>						mShaderVertex;
	vkref<VkShaderModule>						mShaderFragment;
	vkshadermodule*								mShaders[(int)shader_stage::_max]	= {nullptr};

	vkarray<VkVertexInputBindingDescription>	mVertexInputBinding;
	vkarray<VkVertexInputAttributeDescription>	mVertexInputAttributes;


#if	0

	static ref<vkpipeline> realize(pipeline& _pipeline, vkref<VkDevice> const& _device, vkref<VkPipelineCache> const& _cache)
	{
		return	_mat->realize<vkmaterial>([&]()
		{
			ref<vkmaterial>	thiz	= new vkmaterial();

			thiz->mFormat	= vkpipelinelayout::realize(_mat->format(), _device);
			for (int i = 0; i < (int)shader_stage::_max; ++i)
			{
				if (auto sm = _mat->shader((shader_stage)i); sm)
				{
					thiz->mShaders[i]	= vkshadermodule::realize(sm, _device);
				}
			}

			thiz->mCache			= _cache;
			thiz->mLayout			= thiz->mFormat->mLayout;
			thiz->mSetLayout		= thiz->mFormat->mSetLayout;
			thiz->mShaderVertex		= thiz->shader_module(shader_stage::vertex_stage);
			thiz->mShaderFragment	= thiz->shader_module(shader_stage::fragment_stage);
			thiz->input_format(_mat->format()->inputformat().data(), _mat->format()->inputformat().size());

			return	thiz;
		});
	}

	vkref<VkPipeline> const& pipeline(vkref<VkRenderPass> _renderpass, VkExtent2D _viewsize)
	{
		// 前回の呼び出しから変わっていなければパイプラインの生成はスキップする
		if (!mRenderpass.empty() && mRenderpass.is_compatible(_renderpass))
		{
			return	mPipeline;
		}

		// パイプラインを作り直す
		mRenderpass	= _renderpass;

		// 頂点シェーダーへの入力情報
		VkPipelineVertexInputStateCreateInfo	vertex
		{
			.sType								= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext								= nullptr,
			.flags								= 0,
			.vertexBindingDescriptionCount 		= static_cast<uint32_t>(mVertexInputBinding.size()),
			.pVertexBindingDescriptions			= mVertexInputBinding.data(),
			.vertexAttributeDescriptionCount  	= static_cast<uint32_t>(mVertexInputAttributes.size()),
			.pVertexAttributeDescriptions		= mVertexInputAttributes.data(),
		};

		// 入力アセンブリ
		VkPipelineInputAssemblyStateCreateInfo	iasci
		{
			.sType					= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext					= nullptr,
			.flags					= 0,
			.topology				= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable	= VK_FALSE,
		};

		// ラスタライズ
		VkPipelineRasterizationStateCreateInfo	rsci
		{
			.sType						= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.pNext						= nullptr,
			.flags						= 0,
			.depthClampEnable			= VK_FALSE,
			.rasterizerDiscardEnable	= VK_FALSE,
			.polygonMode				= VK_POLYGON_MODE_FILL,
			.cullMode					= VK_CULL_MODE_NONE,		// VK_CULL_MODE_BACK_BIT
			.frontFace					= VK_FRONT_FACE_CLOCKWISE,
			// depth
			.depthBiasEnable			= VK_FALSE,
			.depthBiasConstantFactor	= 0,
			.depthBiasClamp				= 0,
			.depthBiasSlopeFactor		= 0,
			//
			.lineWidth					= 1,
		};

		// カラーブレンド
		VkPipelineColorBlendAttachmentState		cba[]
		{
			{
				.blendEnable			= VK_FALSE,
				.srcColorBlendFactor	= VK_BLEND_FACTOR_ZERO,
				.dstColorBlendFactor	= VK_BLEND_FACTOR_ZERO,
				.colorBlendOp			= VK_BLEND_OP_ADD,
				.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO,
				.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO,
				.alphaBlendOp			= VK_BLEND_OP_ADD,
				.colorWriteMask			= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			}
		};

		VkPipelineColorBlendStateCreateInfo		cbsci
		{
			.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext				= nullptr,
			.flags				= 0,
			.logicOpEnable		= VK_FALSE,
			.logicOp 			= VK_LOGIC_OP_NO_OP,
			.attachmentCount	= 1,
			.pAttachments		= cba,
			.blendConstants		= {1.0f,1.0f,1.0f,1.0f,},
		};

		// ビューポートとシザー
		VkViewport viewports
		{
			.x			= 0,
			.y			= 0,
			.width		= (float)_viewsize.width,
			.height		= (float)_viewsize.height,
			.minDepth	= 0,
			.maxDepth	= 1,
		};;
		VkRect2D	scissor
		{
			.offset	= { 0, 0 },
			.extent	= { _viewsize.width, _viewsize.height },
		};
		VkPipelineViewportStateCreateInfo		vsci
		{
			.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext			= nullptr,
			.flags			= 0,
			.viewportCount	= 1,
			.pViewports		= &viewports,
			.scissorCount	= 1,
			.pScissors		= &scissor,
		};

		// Depth & Stencil
		VkPipelineDepthStencilStateCreateInfo	dssci
		{
			.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.pNext					= NULL,
			.flags					= 0,
			.depthTestEnable		= VK_TRUE,
			.depthWriteEnable		= VK_TRUE,
			.depthCompareOp			= VK_COMPARE_OP_LESS_OR_EQUAL,
			.depthBoundsTestEnable	= VK_FALSE,
			.stencilTestEnable		= VK_FALSE,
			.front					= VkStencilOpState 
			{
				.failOp			= VK_STENCIL_OP_KEEP,
				.passOp			= VK_STENCIL_OP_KEEP,
				.depthFailOp	= VK_STENCIL_OP_KEEP,
				.compareOp		= VK_COMPARE_OP_ALWAYS,
				.compareMask	= 0,
				.writeMask		= 0,
				.reference		= 0,
			},
			.back					= VkStencilOpState 
			{
				.failOp			= VK_STENCIL_OP_KEEP,
				.passOp			= VK_STENCIL_OP_KEEP,
				.depthFailOp	= VK_STENCIL_OP_KEEP,
				.compareOp		= VK_COMPARE_OP_ALWAYS,
				.compareMask	= 0,
				.writeMask		= 0,
				.reference		= 0,
			},
			.minDepthBounds			= 0,
			.maxDepthBounds			= 1,
		};

		// サンプラー
		VkPipelineMultisampleStateCreateInfo	msci
		{
			.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.pNext					= nullptr,
			.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable	= VK_FALSE,
			.minSampleShading		= 0,
			.pSampleMask			= nullptr,
			.alphaToCoverageEnable	= VK_FALSE,
			.alphaToOneEnable		= VK_FALSE,
		};

		// シェーダー
		VkPipelineShaderStageCreateInfo			stage[]
		{
			mShaderVertex	.shader_stage(VK_SHADER_STAGE_VERTEX_BIT, "main"),
			mShaderFragment	.shader_stage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"),
		};

		VkGraphicsPipelineCreateInfo		ci
		{
			.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext					= nullptr,
			.flags					= 0,
			.stageCount				= 2,
			.pStages				= stage,
			.pVertexInputState		= &vertex,
			.pInputAssemblyState	= &iasci,
			.pTessellationState		= nullptr,
			.pViewportState			= &vsci,
			.pRasterizationState	= &rsci,
			.pMultisampleState		= &msci,
			.pDepthStencilState 	= &dssci,
			.pColorBlendState		= &cbsci,
			.pDynamicState			= nullptr,
			.layout					= mLayout,
			.renderPass				= mRenderpass,
			.subpass				= 0,
			.basePipelineHandle		= nullptr,
			.basePipelineIndex		= 0,
		};

		// 変更あり
		mRenderpass	= _renderpass;

		mPipeline	= vkref<VkPipeline>::create(std::get<VkDevice>(mCache), {}, &ci);

		return	mPipeline;
	}


	void input_format(g3d::attribute const* _attr, size_t _size)
	{
		// stride を計算
		auto	bind	= vkarray<VkVertexInputBindingDescription>(1);
		auto	attr	= vkarray<VkVertexInputAttributeDescription>(_size);


		uint32_t	offset		= 0;
		uint32_t	location	= 0;
		for (uint32_t i = 0; i < _size; ++i)
		{
			attr[i]	= 
			{
				.location	= location,
				.binding	= 0,
				.format		= (_attr[i].type == g3d::element_type::float_value && _attr[i].count == 4) ? VK_FORMAT_R32G32B32A32_SFLOAT
							: (_attr[i].type == g3d::element_type::float_value && _attr[i].count == 3) ? VK_FORMAT_R32G32B32_SFLOAT
							: (_attr[i].type == g3d::element_type::float_value && _attr[i].count == 2) ? VK_FORMAT_R32G32_SFLOAT
							: (_attr[i].type == g3d::element_type::float_value && _attr[i].count == 1) ? VK_FORMAT_R32_SFLOAT
							: VK_FORMAT_R32_SFLOAT,
				.offset		= offset,
			};
			++location;
			offset	+= static_cast<uint32_t>((_attr[i].type == g3d::element_type::float_value ? sizeof(float) : 1) * _attr[i].count);
		}
		bind[0]	= 
		{
			.binding	= 0,
			.stride		= offset,
			.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX,
		};

		mVertexInputBinding		= std::move(bind);
		mVertexInputAttributes	= std::move(attr);
	}


	vkshadermodule* shader(shader_stage _stage)
	{
		return	mShaders[(int)_stage];
	}

	vkref<VkShaderModule> shader_module(shader_stage _stage)
	{
		if (auto sm = shader(_stage); sm)
		{
			return	sm->mModule;
		}
		return	{};
	}

	vkref<VkPipelineLayout> layout()
	{
		return	mFormat->mLayout;
	}

	vkref<VkDescriptorSetLayout> descset_layout()
	{
		return	mFormat->mSetLayout;
	}

protected:
	vkref<VkRenderPass>							mRenderpass;
	vkref<VkPipeline>							mPipeline;		// パイプライン本体
	vkref<VkPipelineCache>						mCache;			// 関連付けているキャッシュ

	vkpipelinelayout*							mFormat;
	vkref<VkPipelineLayout>						mLayout;
	vkref<VkDescriptorSetLayout>				mSetLayout;
	vkref<VkShaderModule>						mShaderVertex;
	vkref<VkShaderModule>						mShaderFragment;
	vkshadermodule*								mShaders[(int)shader_stage::_max]	= {nullptr};

	vkarray<VkVertexInputBindingDescription>	mVertexInputBinding;
	vkarray<VkVertexInputAttributeDescription>	mVertexInputAttributes;
#endif
};

AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
