/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/graphics/g3d.hpp>
#include <amtrs/system/log.hpp>
AMTRS_G3D_NAMESPACE_BEGIN namespace basicshader {
/*
material fillshader::create(commandlist* _updatecmd, api_type _api)
{
	static api_type		sLastApi	= static_cast<api_type>(~0);
	static material		sCache;
	#include <amtrs/.inc/g3d-shader/vulkan.fillshader.vert.hpp>
	#include <amtrs/.inc/g3d-shader/vulkan.fillshader.frag.hpp>

	if (sCache.empty())
	{
		sCache	= material(
			// layout
			{
				// Vertex/Input
				vertex::attribute_info,
				// Uniform/Constants
				{
					{ shader_stage::vertex_stage, uniform::attributes }
				}
			},
			// shaders
			{
				shadermodule(shader_stage::vertex_stage),
				shadermodule(shader_stage::fragment_stage)
			}
		);
		_updatecmd->update(sCache.shader(shader_stage::vertex_stage), shared_buffer<char>(std::begin(vulkan_fillshader_vert_spv), std::end(vulkan_fillshader_vert_spv)));
		_updatecmd->update(sCache.shader(shader_stage::fragment_stage), shared_buffer<char>(std::begin(vulkan_fillshader_frag_spv), std::end(vulkan_fillshader_frag_spv)));
	}
	return	sCache;
}



material textureshader::create(commandlist* _updatecmd, api_type _api)
{
	static api_type		sLastApi	= static_cast<api_type>(~0);
	static material		sCache;
	#include <amtrs/.inc/g3d-shader/vulkan.textureshader.vert.hpp>
	#include <amtrs/.inc/g3d-shader/vulkan.textureshader.frag.hpp>

	if (sCache.empty())
	{
		sCache	= material(
			// layout
			{
				// Vertex/Input
				vertex::attribute_info,
				// Uniform/Constants
				{
					{ shader_stage::vertex_stage, uniform::attributes },
					{ shader_stage::fragment_stage,	format_types::texture_value },
				}
			},
			// shaders
			{
				shadermodule(shader_stage::vertex_stage),
				shadermodule(shader_stage::fragment_stage)
			}
		);
		_updatecmd->update(sCache.shader(shader_stage::vertex_stage), shared_buffer<char>(std::begin(vulkan_textureshader_vert_spv), std::end(vulkan_textureshader_vert_spv)));
		_updatecmd->update(sCache.shader(shader_stage::fragment_stage), shared_buffer<char>(std::begin(vulkan_textureshader_frag_spv), std::end(vulkan_textureshader_frag_spv)));
	}
	return	sCache;
}
*/

} AMTRS_G3D_NAMESPACE_END
