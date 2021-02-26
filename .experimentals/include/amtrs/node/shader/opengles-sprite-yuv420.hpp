/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__shader__opengles_sprite_yuv420__hpp
#define	__libamtrs__node__shader__opengles_sprite_yuv420__hpp
#include "../../g3d/shader.hpp"
AMTRS_NAMESPACE_BEGIN

class	opengles_sprite_yuv420_shader
		: public g3d::shader
{
public:
	struct	vertex
	{
		using	color_type		= rgba<float>;
		vec4<float>		pos;
		vec2<float>		uv;
		color_type		col;

		static constexpr g3d::attribute		attribute_info[] = 
		{
			g3d::attribute::make<decltype(pos)>("vertex_pos"),
			g3d::attribute::make<decltype(uv)>("vertex_uv"),
			g3d::attribute::make<decltype(col)>("vertex_color")
		};
	};


	struct	uniform
	{
		mat44<float>		perspective;
		mat44<float>		coordinate;
		rgba<float>			color			= {1, 1, 1, 1};
		ref<g3d::texture>	texture_y		= nullptr;
		ref<g3d::texture>	texture_u		= nullptr;
		ref<g3d::texture>	texture_v		= nullptr;

		static constexpr g3d::attribute		uniform_info[] = 
		{
			g3d::attribute::make<decltype(perspective)>("perspective"),
			g3d::attribute::make<decltype(coordinate)>("coordinate"),
			g3d::attribute::make<decltype(color)>("color"),
			g3d::attribute::make<decltype(texture_y)>("texture1"),
			g3d::attribute::make<decltype(texture_u)>("texture2"),
			g3d::attribute::make<decltype(texture_v)>("texture3"),
		};
		static_assert(sizeof(ref<g3d::texture>) == sizeof(void*));
	};


	static ref<opengles_sprite_yuv420_shader> create()
	{
		if (!sInstance)
		{
			sInstance	= new opengles_sprite_yuv420_shader();
			sInstance->queue([](auto mem)
			{
				mem->compile(
					vertex::attribute_info,
					uniform::uniform_info,
					// Vertex shader
					std::string(
					//	"#version 130\n"
						"attribute vec4		vertex_pos;\n"
						"attribute vec2		vertex_uv;\n"
						"attribute vec4		vertex_color;\n"
						"uniform   mat4     perspective;\n"
						"uniform   mat4     coordinate;\n"
						"uniform   vec4		color;\n"
						"varying  vec4		pixel_color;\n"
						"varying  vec2		tex_uv;\n"
						"void main() {\n"
						"	gl_Position = perspective * coordinate * vertex_pos;\n"
						"	tex_uv		= vertex_uv;\n"
						"	pixel_color	= color * vertex_color;\n"
						"}\n"
					),
					// Fragment shader
					std::string(
					//	"#version 130\n"
						"precision highp   float;\n"
						"uniform sampler2D	texture1;\n"
						"uniform sampler2D	texture2;\n"
						"uniform sampler2D	texture3;\n"
						"varying  vec2		tex_uv;\n"
						"varying  vec4		pixel_color;\n"
						"void main() {\n"
						"	float y, u, v, r, g, b;\n"
						"	y = texture2D(texture1, tex_uv).r;\n"
						"	u = texture2D(texture2, tex_uv).r - 0.5;\n"
						"	v = texture2D(texture3, tex_uv).r - 0.5;\n"
						"	r = y + 1.403 * v;\n"
						"	g = y - 0.344 * u - 0.714 * v;\n"
						"	b = y + 1.770 * u;\n"
						"	gl_FragColor = vec4(r, g, b, 1.0) * pixel_color;\n"
						"}\n"
					)
				);
			});
		}
		return	sInstance;
	}

private:
	~opengles_sprite_yuv420_shader()
	{
		sInstance	= nullptr;
	}

	static inline opengles_sprite_yuv420_shader*	sInstance;
};
AMTRS_NAMESPACE_END
#endif
