/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vkpipelinelayout__hpp
#define	__libamtrs__driver__g3d__vulkan__vkpipelinelayout__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN




//! 定数レイアウト
class	vkpipelinelayout
		: public ref_object
{
public:
	static constexpr VkShaderStageFlags vkstage(shader_stage _stage) noexcept
	{
		return	_stage	== shader_stage::vertex_stage		? VK_SHADER_STAGE_VERTEX_BIT
			:	_stage	== shader_stage::tesslation_stage	? VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
			:	_stage	== shader_stage::geometory_stage	? VK_SHADER_STAGE_GEOMETRY_BIT
			:	_stage	== shader_stage::fragment_stage		? VK_SHADER_STAGE_FRAGMENT_BIT
			:	_stage	== shader_stage::compute_stage		? VK_SHADER_STAGE_COMPUTE_BIT
			:	0;
	}

	static ref<vkpipelinelayout> realize(material_layout& _format, vkref<VkDevice> const& _device)
	{
		return	realizing<vkpipelinelayout>(_format, _device);
	}

	void initialize(material_layout::instance_type* _owner, vkref<VkDevice> const& _device)
	{
		mOwner	= _owner;

		// Uniforms をリストアップ
		auto		uniforms	= mOwner->uniforms;
		auto		bindings	= vkarray<VkDescriptorSetLayoutBinding>(uniforms.size());
		uint32_t	i			= 0;
		for (auto& u : uniforms)
		{
			bindings[i]	= 
			{
				.binding			= i,
				.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount	= 1,
				.stageFlags			= vkstage(u.stage),
				.pImmutableSamplers	= nullptr,
			};
			if (u.type == format_types::texture_value)
			{
				bindings[i].descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			}

			AMTRS_ERROR_LOG("Bindings : binding=%d/%d desc=%s, stage=%s", (int)bindings[i].binding, (int)bindings.size(), std::to_string(bindings[i].descriptorType).c_str(), std::to_string((VkShaderStageFlagBits)bindings[i].stageFlags).c_str());

			++i;
		}

		mSetLayout	= vkref<VkDescriptorSetLayout>::create(_device, bindings);
		mLayout		= vkref<VkPipelineLayout>::create(mSetLayout);
	}

	material_layout::instance_type*	mOwner;
	vkref<VkDescriptorSetLayout>	mSetLayout;
	vkref<VkPipelineLayout>			mLayout;
	size_t							mSize;
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
