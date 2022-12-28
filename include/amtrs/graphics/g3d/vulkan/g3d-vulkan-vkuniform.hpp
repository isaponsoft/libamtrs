/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vkuniform__hpp
#define	__libamtrs__driver__g3d__vulkan__vkuniform__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


class	vkuniform
		: public ref_object
{
public:
	union	descinfo
	{
		VkDescriptorBufferInfo	buff;
		VkDescriptorImageInfo	img;
	};

	void initialize(uniformset::instance_type* _uniform, vkref<VkDevice> const& _device)
	{
		mUniform		= _uniform;
		auto	layout	= vkpipelinelayout::realize(mUniform->layout, _device);
		mLayout			= layout->mLayout;
		mUniformDesc	= vkref<VkDescriptorSet>::create(layout->mSetLayout, 1)[0];
		mKeeps			= shared_buffer<ref<ref_object>>(mUniform->layout.uniforms().size());
	}


	void update(vkref<VkDevice> const& _device)
	{
		if (!mUniform->modified)
		{
			return;
		}

		// ディスクリプタを更新する
		shared_buffer<VkWriteDescriptorSet>	descset(mUniform->binds.size());
		shared_buffer<descinfo>				infos(mUniform->binds.size());
		VkWriteDescriptorSet*				desc	= descset.data();
		auto*								info	= infos.data();
		auto*								bind	= mUniform->binds.data();
		uint32_t							binding	= 0;
		for (auto u : mUniform->layout.uniforms())
		{
			desc[binding]		= VkWriteDescriptorSet
			{
				/*.sType			= */VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				/*.pNext			= */nullptr,
				/*.dstSet			= */mUniformDesc,
				/*.dstBinding		= */binding,
				/*.dstArrayElement	= */0,
				/*.descriptorCount	= */1,
				/*.descriptorType 	= */VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				/*.pImageInfo 		= */nullptr,
				/*.pBufferInfo 		= */nullptr,
				/*.pTexelBufferView	= */nullptr,
			};

			switch (u.type)
			{
				case format_types::constants_value :
				{
					auto			data	= static_cast<uniformset::instance_type::constants_data*>(bind[binding].get());
					vkref<VkBuffer>	buff	= vkconstants::realize(data->val1, _device)->mBuffer;
					info[binding].buff	= VkDescriptorBufferInfo
					{
						/*.buffer		= */buff,
						/*.offset		= */0,
						/*.range		= */buff.size(),
					}; 
					desc[binding].descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					desc[binding].pBufferInfo		= &info[binding].buff;
					break;
				}

				case format_types::texture_value :
				{
					auto		data	= static_cast<uniformset::instance_type::tetxure_data*>(bind[binding].get());
					auto		tex		= vktexture::realize(data->val1, _device);
					vksampler*	samp	= vksampler::realize(data->val2, _device);

					VkImageViewCreateInfo	vci
					{
						/*.sType				=*/VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
						/*.pNext				=*/nullptr,
						/*.flags				=*/0,
						/*.image				=*/tex->mImage,
						/*.viewType			=*/VK_IMAGE_VIEW_TYPE_2D,
						/*.format				=*/tex->mImage.format(),
						//.components			= VkComponentMapping
						{
						/*	.r				=*/VK_COMPONENT_SWIZZLE_R,
						/*	.g				=*/VK_COMPONENT_SWIZZLE_G,
						/*	.b				=*/VK_COMPONENT_SWIZZLE_B,
						/*	.a				=*/VK_COMPONENT_SWIZZLE_A,
						},
						//.subresourceRange	= VkImageSubresourceRange
						{
						/*	.aspectMask		=*/VK_IMAGE_ASPECT_COLOR_BIT,
						/*	.baseMipLevel	=*/0,
						/*	.levelCount		=*/1,
						/*	.baseArrayLayer	=*/0,
						/*	.layerCount		=*/1,
						},
					};

					auto			view	= vkref<VkImageView>::create(tex->mImage, vci);
					info[binding].img	= VkDescriptorImageInfo
					{
						/*.sampler		= */samp->mSampler,
						/*.imageView		= */view,
						/*.imageLayout	= */VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,	//VK_IMAGE_LAYOUT_GENERAL;
					};

					desc[binding].descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					desc[binding].pImageInfo		= &info[binding].img;
					mKeeps[binding]					= view.handle();
					break;
				}
			}
			++binding;
		}
		vkUpdateDescriptorSets(_device, static_cast<uint32_t>(descset.size()), descset.data(), 0, nullptr);
		mUniform->modified	= false;
	}

	void exec(vkref<VkCommandBuffer> const& _cmdbuff)
	{
		VkDescriptorSet	sets[] = { mUniformDesc };
		vkCmdBindDescriptorSets(_cmdbuff, VK_PIPELINE_BIND_POINT_GRAPHICS, mLayout, 0, 1, sets, 0, nullptr);
	}


	uniformset::instance_type*		mUniform	= nullptr;
	vkref<VkPipelineLayout>			mLayout;
	vkref<VkDescriptorSet>			mUniformDesc;
	shared_buffer<ref<ref_object>>	mKeeps;
};
AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
