/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__graphics__opengl__gl_renderer__hpp
#define	__libamtrs__graphics__opengl__gl_renderer__hpp
#include <iostream>
#include "../renderer.hpp"
#include "def.hpp"
AMTRS_OPENGL_NAMESPACE_BEGIN 


// ============================================================================
//! OpenGL のコンテキストを抽象化したインターフェースです。
// ----------------------------------------------------------------------------
//! コンテキストの作成方法はＯＳによって異なります。
//! Win32なら amtrs::os::win32::opengl、Android なら amtrs::os::android::opengl
//! などにあるコンテキストヘルパで生成できます。
// ----------------------------------------------------------------------------
class	opengl_renderer
		: public renderer
{
public:
	static constexpr GLuint		noshader	= static_cast<GLuint>(-1);

	opengl_renderer()
		: renderer(opengl)
	{}

	virtual std::string name() const override
	{
		std::string	retval;
		auto	vendor	= reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		auto	render	= reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		retval	+= (vendor ? vendor : "unkown vendor");
		retval	+= '/';
		retval	+= (render ? render : "unkown render");
		return	retval;
	}


	virtual ref<shader> create_shader(std::string_view _vertex, std::string_view _fragment) override
	{
		class	opengl_shader
				: public shader
		{
		public:

			void initialize(std::string_view _vertex, std::string_view _fragment)
			{
				mVtxShader = compile(GL_VERTEX_SHADER,   _vertex  );
				mFrgShader = compile(GL_FRAGMENT_SHADER, _fragment);

				// 二つをリンク
				GLint		linkStatus	= GL_FALSE;
				mShader					= glCreateProgram();
				glAttachShader(mShader, mVtxShader);
				glAttachShader(mShader, mFrgShader);
				glLinkProgram(mShader);
				glGetProgramiv(mShader, GL_LINK_STATUS, &linkStatus);
				if (!linkStatus)
				{
					char	temp[512+1] = "";
					glGetShaderInfoLog(mShader, sizeof(temp)-1, nullptr, temp);
					throw	std::logic_error(temp);
				}
			}


		private:
			GLuint compile(GLenum _shaderType, std::string_view _program)
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
					throw	std::runtime_error(temp);
				}
				return	shader;
			}


			GLuint	mShader		= noshader;
			GLuint	mVtxShader	= noshader;
			GLuint	mFrgShader	= noshader;
		};

		ref<opengl_shader>	thiz	= new opengl_shader();
		thiz->initialize(_vertex, _fragment);
		return	thiz;
	}

};


AMTRS_OPENGL_NAMESPACE_END
#endif
