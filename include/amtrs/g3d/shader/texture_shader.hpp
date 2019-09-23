/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__g3dex__shader__texture_shader__hpp
#define __libamtrs__g3dex__shader__texture_shader__hpp
#include "../component/shader.hpp"
#include "def.hpp"
#include "vertex.hpp"
#include "uniform.hpp"
AMTRS_G3DEX_SHADER_NAMESPACE_BEGIN



struct	texture_shader
		: g3d::component::shader<texture_vertex, texture_uniform>
{
	g3d::shader* get_shader(g3d::device* _device)
	{
		using	bsae_type	= g3d::component::shader<texture_vertex, texture_uniform>;
		return	bsae_type::get_shader
				(
					_device, 
					// Vertex Shader
					{
						"attribute vec4		vertex_pos;\n"
						"attribute vec2		vertex_uv;\n"
						"attribute vec4		vertex_color;\n"
						"uniform   vec4		color;\n"
						"uniform   mat4     perspective;\n"
						"uniform   mat4     coordinate;\n"
						"varying  vec4		pixel_color;\n"
						"varying  vec2		tex_uv;\n"
						"void main() {\n"
						"	gl_Position = perspective * coordinate * vertex_pos;\n"
						"	tex_uv		= vertex_uv;\n"
						"	pixel_color	= color * vertex_color;\n"
						"}\n"
					},
					// Fragment Shader
					{
						"precision highp   float;\n"
						"uniform sampler2D	texture1;\n"
						"varying  vec2		tex_uv;\n"
						"varying  vec4		pixel_color;\n"
						"void main() {\n"
						" 	gl_FragColor = texture2D(texture1, tex_uv) * pixel_color;\n"
						"}\n"
					}
				);
	}
};


AMTRS_G3DEX_SHADER_NAMESPACE_END
#endif
