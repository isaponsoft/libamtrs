#ifndef __libamtrs__g3d__command__splite_list__hpp
#define __libamtrs__g3d__command__splite_list__hpp
#include "def.hpp"
AMTRS_G3D_NAMESPACE_BEGIN

class	sprite_list
		: public command
{
public:

	struct	Uniforms
	{
		Graphics::Shader::Uniform		texture;
	};

	struct	Vertex
	{
		vec4<float>			pos;
		Math::Vector2		uv;
		rgba_type			color;
	};


	Uniforms					uniforms;
	std::vector<Vertex>			vertexList;
	mat44<float>				matrix;


	void set_texture(texture_type* _texture)
	{
		mTextures.push_back(_texture);
	}

	virtual void on_precommand(device_type& _device) override
	{
		static		Graphics::Shader::IProgram	s_program;

		// シェーダープログラムを構築済みなら何もしない
		if (mProgram)
		{
			return;
		}

		// シェーダープログラムを作成する
		mProgram	= s_program;
		if (!mProgram)
		{
			static const char	vsp[] = 
				"attribute vec4		vertex_pos;\n"
				"attribute vec2		vertex_uv;\n"
				"attribute vec4		vertex_color;\n"
				"varying vec2		pixel_uv;\n"
				"varying vec4		pixel_color;\n"
				"void main() {\n"
				"	gl_Position = vertex_pos;\n"
				"	pixel_uv	= vertex_uv;\n"
				"	pixel_color	= vertex_color;\n"
				"}\n";

			static const char	psp[] = 
				"precision mediump float;\n"
				"uniform sampler2D	texture1;\n"
				"varying vec2		pixel_uv;\n"
				"varying vec4		pixel_color;\n"
				"void main() {\n"
				" 	gl_FragColor = texture2D(texture1, pixel_uv) * pixel_color;\n"//vec4(pixel_uv, 1, 1);\n"
				"}\n";


			// シェーダーを生成
			Graphics::Shader::Program::Create(&mProgram, &_device, vsp, psp);
		}

		setup(_device);
		mProgram->GetUniform(&uniforms.texture, "texture1");

		Graphics::Shader::VertexAttribute	attributes[3];

		attributes[0].name		= "vertex_pos";
		attributes[0].type		= Graphics::Shader::Attribute::eVector4;
		attributes[0].offset	= 0;

		attributes[1].name		= "vertex_uv";
		attributes[1].type		= Graphics::Shader::Attribute::eVector2;
		attributes[1].offset	= 0;

		attributes[2].name		= "vertex_color";
		attributes[2].type		= Graphics::Shader::Attribute::eVector4;
		attributes[2].offset	= 0;
		Graphics::Shader::VertexAttribute::ComputeOffset(attributes, 3);

		mProgram->SetAttributes(sizeof(Vertex), attributes, 3);

		s_program	= mProgram;
	}

	virtual void on_render(renderer_type& _render) override
	{
		// シェーダープログラムと頂点アトリビュートを設定
		_render.Set(mProgram);
		_render.Set(&uniforms.texture, mTextures.data(), mTextures.size());
		_render.Draw(Graphics::Shader::eTriangleList, vertexList.data(), 0, (unsigned int)vertexList.size());
	}
};

AMTRS_G3D_NAMESPACE_END
#endif
