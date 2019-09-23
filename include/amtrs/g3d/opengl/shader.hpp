#ifndef	__libamtrs__graphics__opengl__shader__hpp
#define	__libamtrs__graphics__opengl__shader__hpp
#include "attribute.hpp"
#include "notify.hpp"
#include "texture_memory.hpp"
AMTRS_OPENGL_NAMESPACE_BEGIN



class	shader
		: public g3d::shader
{
public:
	static constexpr GLuint	novalue	= static_cast<GLuint>(0);
	virtual ~shader()
	{
		release();
	}

	const vertex_layout& get_vertex_layout() const noexcept { return mVertexLayout; }
	void set_vertex_layout(const vertex_layout& _vl) { mVertexLayout = _vl; }

	virtual bool is_enable() const noexcept override
	{
		return	mShader != novalue;
	}


	template<class Resources>
	void load_resource(const g3d::device_capacity& _capacity, const void* _uniforms, Resources* _resources)
	{
		GLuint	textureIndex	= 0;
		for (const auto& uni : mUniformLayout)
		{
			if (uni.type == GL_TEXTURE_2D)
			{
				auto	tex			= *reinterpret_cast<g3d::texture**>((std::uintptr_t)_uniforms + uni.offset);
				texture_memory::active(GL_TEXTURE0 + textureIndex, _capacity, tex, _resources);
				++textureIndex;
			}
		}
	}


	void transfer_uniform(const g3d::device_capacity& _capacity, const void* _uniforms)
	{
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

	void deactive()
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

	void release()
	{
		if (mShader != novalue)
		{
			glDeleteShader(mShader);
		}
		if (mVtxShader != novalue)
		{
			glDeleteShader(mVtxShader);
		}
		if (mFrgShader != novalue)
		{
			glDeleteShader(mFrgShader);
		}
		mShader		= novalue;
		mVtxShader	= novalue;
		mFrgShader	= novalue;
	}

	virtual void on_gain() override
	{
		create();
	}

	virtual void on_lost() override
	{
		release();
	}

	void create()
	{
		release();
		(void)glGetError();
		mVtxShader = compile(GL_VERTEX_SHADER,   mVertexProgram);
		if (mVtxShader == novalue)
		{
			AMTRS_DEBUG_LOG("mVtxShader error");
		}
		mFrgShader = compile(GL_FRAGMENT_SHADER, mFragmentProgram);
		if (mFrgShader == novalue)
		{
			AMTRS_DEBUG_LOG("mFrgShader error");
		}

		// 二つをリンク
		GLint		linkStatus	= GL_FALSE;
		mShader					= glCreateProgram();
		if (mShader == novalue)
		{
			AMTRS_DEBUG_LOG("mShader error");
		}
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
		glDeleteShader(mVtxShader);
		glDeleteShader(mFrgShader);
		mVtxShader	= novalue;
		mFrgShader	= novalue;
	}


	void initialize(std::string_view _vertex, std::string_view _fragment)
	{
		mVertexProgram		= _vertex;
		mFragmentProgram	= _fragment;
		create();
	}


	void initialize_attribute_info(const attribute* _attr, std::size_t _attr_size)
	{
		mVertexLayout	= vertex_layout::create_from_shader(mShader);
		mVertexLayout.mapping(_attr, _attr_size);
	}


	void initialize_uniform_info(const attribute* _uniform, std::size_t _uniform_size)
	{
		mUniformLayout	= uniform_layout::create_from_shader(mShader);
		mUniformLayout.mapping(_uniform, _uniform_size);
	}
	

	void use_program()
	{
		AMTRS_G3D_OPENGL_GLGETERROR("Before glUseProgram")

		glUseProgram(mShader);
		AMTRS_G3D_OPENGL_GLGETERROR("glUseProgram")
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
            AMTRS_DEBUG_LOG("Shader compile error: %s", temp);
			throw	std::runtime_error(format<std::string>("shader compile error: %s", temp));
		}
		return	shader;
	}


	std::string				mVertexProgram;
	std::string				mFragmentProgram;
	GLuint					mShader		= novalue;
	GLuint					mVtxShader	= novalue;
	GLuint					mFrgShader	= novalue;
	vertex_layout			mVertexLayout;
	uniform_layout			mUniformLayout;
};

AMTRS_OPENGL_NAMESPACE_END
#endif
