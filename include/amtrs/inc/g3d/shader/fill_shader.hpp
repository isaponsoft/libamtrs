/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__g3dex__shader__fill_shader__hpp
#define __libamtrs__g3dex__shader__fill_shader__hpp
#include "def.hpp"
#include "vertex.hpp"
#include "uniform.hpp"
AMTRS_G3DEX_SHADER_NAMESPACE_BEGIN



struct	fill_shader
		: g3d::component::shader<color_vertex, simple_uniform>
{
	g3d::shader* get_shader(g3d::device* _device)
	{
		using	bsae_type	= g3d::component::shader<color_vertex, simple_uniform>;
		return	bsae_type::get_shader
				(
					_device, 
					// Vertex Shader
					{
						"attribute vec4		vertex_pos;\n"
						"attribute vec4		vertex_color;\n"
						"uniform   mat4     perspective;\n"
						"uniform   mat4     coordinate;\n"
						"varying  vec4		pixel_color;\n"
						"void main() {\n"
						"	gl_Position = perspective * coordinate * vertex_pos;\n"
						"	pixel_color	= vertex_color;\n"
						"}\n"
					},
					// Fragment Shader
					{
						"precision mediump float;\n"
						"varying  vec4		pixel_color;\n"
						"void main() {\n"
						" 	gl_FragColor = pixel_color;\n"
						"}\n"
					}
				);
	}
};


AMTRS_G3DEX_SHADER_NAMESPACE_END
#endif
