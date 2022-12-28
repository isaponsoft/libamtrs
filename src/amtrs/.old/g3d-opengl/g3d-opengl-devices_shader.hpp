/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__opengl__devices_shader__hpp
#define	__libamtrs__graphics__opengl__devices_shader__hpp
AMTRS_OPENGL_NAMESPACE_BEGIN


class	devices_shader
		: g3d::shader::memory
{
public:
	static constexpr GLuint	novalue	= static_cast<GLuint>(0);


	template<class Resources>
	static devices_shader* get_shader(g3d::shader* _shader, Resources* _resources)
	{
		devices_shader*	thiz;
		if (auto m = _shader->get_memory(); m)
		{
			// すでにメモリが存在する
			thiz	= static_cast<devices_shader*>(m);
		}
		else
		{
			// 新しくメモリを確保
			thiz	= new devices_shader();
			_shader->set_memory(thiz);
			_resources->set(thiz);
		}

		// シェーダーがまだ作られていないかロストしているなら再度作成する
		if (thiz->mShader == novalue)
		{
			_shader->compile();
		}
		return	thiz;
	}


	void activate(const g3d::device_capacity& _capacity, const void* _uniforms)
	{
		AMTRS_G3D_OPENGL_GLGETERROR("Before glUseProgram")

		glUseProgram(mShader);
		AMTRS_G3D_OPENGL_GLGETERROR("glUseProgram")


		GLuint	textureIndex	= 0;
		for (const auto& uni : mUniformLayout)
		{
			if (uni.type == GL_TEXTURE_2D)
			{
				glUniform1i(uni.location, textureIndex);
				AMTRS_G3D_OPENGL_GLGETERROR("glUniform1i")
				++textureIndex;
			}
			else
			{
				switch (uni.count)
				{
					case 4 :
						glUniform4fv(uni.location, 1, (const GLfloat*)((std::uintptr_t)_uniforms + uni.offset));
						AMTRS_G3D_OPENGL_GLGETERROR("glUniform4fv")
						break;

					case 16 :
						glUniformMatrix4fv(uni.location, 1, GL_FALSE, (const GLfloat*)((std::uintptr_t)_uniforms + uni.offset));
						AMTRS_G3D_OPENGL_GLGETERROR("glUniformMatrix4fv")
						break;
				}
			}
		}
	}



	template<class Callback>
	void enumrate_textures(const void* _uniforms, Callback&& _callback)
	{
		GLuint	textureIndex	= 0;
		for (const auto& uni : mUniformLayout)
		{
			if (uni.type == GL_TEXTURE_2D)
			{
				auto	tex			= *reinterpret_cast<g3d::texture**>((std::uintptr_t)_uniforms + uni.offset);
				_callback(textureIndex, static_cast<g3d::texture*>(tex));
				++textureIndex;
			}
		}
	}



	void deactivate()
	{
		GLuint	textureIndex	= 0;
		for (const auto& uni : mUniformLayout)
		{
			if (uni.type == GL_TEXTURE_2D)
			{
				glActiveTexture(GL_TEXTURE0 + textureIndex);
				AMTRS_G3D_OPENGL_GLGETERROR("glActiveTexture")
				glBindTexture(GL_TEXTURE_2D, 0);
				AMTRS_G3D_OPENGL_GLGETERROR("glBindTexture")
				++textureIndex;
			}
		}
		glUseProgram(0);
		AMTRS_G3D_OPENGL_GLGETERROR("glUseProgram")
	}

	auto const& vtxlayout() const noexcept { return mVertexLayout; }

protected:
	void on_gain() override
	{
	}

	void on_lost() override
	{
		if (mShader != novalue)
		{
			glDeleteShader(mShader);
			mShader		= novalue;
		}
	}

	void on_compile(const attribute* _attrs, std::size_t _attr_size, const attribute* _uniforms, std::size_t _uniform_size, std::string _vertex, std::string _fragment) override
	{
		// シェーダープログラム
		mVertexProgram		= std::move(_vertex);
		mFragmentProgram	= std::move(_fragment);
		mShader				= build(mVertexProgram, mFragmentProgram);

		// 頂点レイアウト
		mVertexLayout		= vertex_layout::create_from_shader(mShader);
		mVertexLayout.mapping(_attrs, _attr_size);

		// 共有データレイアウト
		mUniformLayout		= uniform_layout::create_from_shader(mShader);
		mUniformLayout.mapping(_uniforms, _uniform_size);
	}


private:
	static GLuint build(std::string const& _vtx, std::string const& _frg)
	{
		GLuint	vtxShader;
		GLuint	frgShader;

		(void)glGetError();
		vtxShader = compile(GL_VERTEX_SHADER,   _vtx);
		if (vtxShader == novalue)
		{
			AMTRS_DEBUG_LOG("vtxShader error");
			return	novalue;
		}
		frgShader = compile(GL_FRAGMENT_SHADER, _frg);
		if (frgShader == novalue)
		{
			AMTRS_DEBUG_LOG("frgShader error");
			glDeleteShader(vtxShader);
			return	novalue;
		}

		// 二つをリンク
		GLint		linkStatus	= GL_FALSE;
		GLint		shader		= glCreateProgram();
		if (shader == novalue)
		{
			AMTRS_DEBUG_LOG("shader error");
			glDeleteShader(vtxShader);
			glDeleteShader(frgShader);
			return	novalue;
		}
		glAttachShader(shader, vtxShader);
		glAttachShader(shader, frgShader);
		glLinkProgram(shader);
		glGetProgramiv(shader, GL_LINK_STATUS, &linkStatus);
		if (!linkStatus)
		{
			char	temp[512+1] = "";
			glGetShaderInfoLog(shader, sizeof(temp)-1, nullptr, temp);
			throw	std::logic_error(temp);
		}
		glDeleteShader(vtxShader);
		glDeleteShader(frgShader);

		return	shader;
	}

	static GLuint compile(GLenum _shaderType, std::string_view _program)
	{
		std::string		program(_program);
		GLint			compiled	= GL_FALSE;

		GLuint			shader		= glCreateShader(_shaderType);
        
		const char*		prgs[]		= { program.c_str() };
		glShaderSource(shader, 1, prgs, nullptr);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			char	temp[512+1] = "";
			glGetShaderInfoLog(shader, sizeof(temp)-1, nullptr, temp);
            AMTRS_DEBUG_LOG("Shader compile error: %s", temp);
			throw	std::runtime_error(format<std::string>("shader compile error: %s", temp));
		}
		return	shader;
	}


	std::string				mVertexProgram;
	std::string				mFragmentProgram;
	GLuint					mShader		= novalue;
	vertex_layout			mVertexLayout;
	uniform_layout			mUniformLayout;
};


AMTRS_OPENGL_NAMESPACE_END
#endif
