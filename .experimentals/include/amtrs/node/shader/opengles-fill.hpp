/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__shader__opengles_fill__hpp
#define	__libamtrs__node__shader__opengles_fill__hpp
#include "../../g3d/shader.hpp"
AMTRS_NAMESPACE_BEGIN

class	opengles_fill_shader
		: public g3d::shader
{
public:
	struct	vertex
	{
		using	color_type		= rgba<float>;
		vec4<float>				pos;
		color_type				col;

		static constexpr g3d::attribute		attribute_info[] = 
		{
			g3d::attribute::make<decltype(pos)>("vertex_pos"),
			g3d::attribute::make<decltype(col)>("vertex_color")
		};
	};


	struct	uniform
	{
		mat44<float>		perspective;
		mat44<float>		coordinate;
		rgba<float>			color			= {1, 1, 1, 1};

		static constexpr g3d::attribute		uniform_info[] = 
		{
			g3d::attribute::make<decltype(perspective)>("perspective"),
			g3d::attribute::make<decltype(coordinate)>("coordinate"),
			g3d::attribute::make<decltype(color)>("color")
		};
	};


	static ref<opengles_fill_shader> create()
	{
		if (!sInstance)
		{
			sInstance	= new opengles_fill_shader();
			sInstance->queue([](auto mem)
			{
				mem->compile(
					vertex::attribute_info,
					uniform::uniform_info,
					// Vertex Shader
					{
						//"#version 130\n"
						"attribute vec4		vertex_pos;\n"
						"attribute vec4		vertex_color;\n"
						"uniform   mat4     perspective;\n"
						"uniform   mat4     coordinate;\n"
						"uniform   vec4		color;\n"
						"varying  vec4		pixel_color;\n"
						"void main() {\n"
						"	gl_Position = perspective * coordinate * vertex_pos;\n"
						"	pixel_color	= color * vertex_color;\n"
						"}\n"
					},
					// Fragment Shader
					{
						//"#version 130\n"
						"precision mediump float;\n"
						"varying  vec4		pixel_color;\n"
						"void main() {\n"
						" 	gl_FragColor = pixel_color;\n"
						"}\n"
					}
				);
			});
		}
		return	sInstance;
	}

private:
	~opengles_fill_shader()
	{
		sInstance	= nullptr;
	}

	static inline opengles_fill_shader*	sInstance;
};
AMTRS_NAMESPACE_END
#endif
