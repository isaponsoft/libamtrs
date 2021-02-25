/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__opengl__attribute__hpp
#define	__libamtrs__graphics__opengl__attribute__hpp
#include "../memory/shared_buffer.hpp"
#include "g3d-opengl-def.hpp"
#include "g3d-opengl-gl_error.hpp"
AMTRS_OPENGL_NAMESPACE_BEGIN

struct	attrfmt
{
	std::string	name;
	GLint		location;
	GLenum		type;
	GLuint		count;
	GLuint		offset;
};


struct	vertex_api
{
	std::size_t size(GLuint _shader) const
	{
		GLint	length;
		glGetProgramiv(_shader, GL_ACTIVE_ATTRIBUTES, &length);
		AMTRS_G3D_OPENGL_GLGETERROR("glGetProgramiv")
		return	length;
	}

	void get(GLuint _shader, GLint _index, GLsizei _nameBufferSize, GLsizei* _nameSize, GLint* _valueSize, GLenum* _type, GLchar* _nameBuffer)
	{
		glGetActiveAttrib(_shader, _index, _nameBufferSize, _nameSize, _valueSize, _type, _nameBuffer);
		AMTRS_G3D_OPENGL_GLGETERROR("glGetActiveAttrib")
	}

	GLint location(GLuint _shader, const GLchar* _name)
	{
		return	glGetAttribLocation(_shader, _name);
	}
};


struct	uniform_api
{
	std::size_t size(GLuint _shader) const
	{
		GLint	length;
		glGetProgramiv(_shader, GL_ACTIVE_UNIFORMS, &length);
		AMTRS_G3D_OPENGL_GLGETERROR("glGetProgramiv")
		return	length;
	}

	void get(GLuint _shader, GLint _index, GLsizei _nameBufferSize, GLsizei* _nameSize, GLint* _valueSize, GLenum* _type, GLchar* _nameBuffer)
	{
		glGetActiveUniform(_shader, _index, _nameBufferSize, _nameSize, _valueSize, _type, _nameBuffer);
		AMTRS_G3D_OPENGL_GLGETERROR("glGetActiveUniform")
	}

	GLint location(GLuint _shader, const GLchar* _name)
	{
		return	glGetUniformLocation(_shader, _name);
	}
};


template<class Api>
class	attribute_layout
		: public shared_buffer<attrfmt, std::size_t>
{
public:
	using	super_type	= shared_buffer<attrfmt, std::size_t>;
	using	buffer_type	= shared_buffer<attrfmt, std::size_t>;

	using	super_type::super_type;
	using	super_type::operator ==;
	using	super_type::operator !=;


	//! シェーダーを元にレイアウトを取得します。
	static attribute_layout create_from_shader(GLuint _shader)
	{
		Api					api;
		GLint				length	= static_cast<GLint>(api.size(_shader));
		attribute_layout	retval(length);

		//各属性を取得
		for (GLint i = 0; i < length; ++i)
		{
			GLchar	nameBuff[128 + 1];
			GLsizei	nameLength;
			GLenum	type;
			GLint	valueLength;
			api.get(_shader, i, sizeof(nameBuff)-1,  &nameLength, &valueLength, &type, nameBuff);
			AMTRS_G3D_OPENGL_GLGETERROR("glGetActiveUniform")

			nameBuff[nameLength] = 0;
			retval[i].name		= nameBuff;
			retval[i].location	= api.location(_shader, nameBuff);
			retval[i].type		= type;
		}
		return	retval;
	}


	//! uniformの長さ(byte size)を返す
	std::size_t length() const noexcept { return this->header(); }


	//! ユーザー構造体をマッピングします。
	std::size_t mapping(const attribute* _values, std::size_t _size)
	{
		// 渡された属性をマッピング
		std::size_t		offset	= 0;
		for (std::size_t i = 0; i < _size; ++i)
		{
			const attribute&	ua = _values[i];
			for (auto& a : *this)
			{
				if (a.name != ua.name)
				{
					continue;
				}
				a.type		= ua.type == g3d::element_type::float_value     ? GL_FLOAT
							: ua.type == g3d::element_type::texture2d_value ? GL_TEXTURE_2D
							: GL_FLOAT;
				a.count		= static_cast<GLuint>(ua.count);
				a.offset	= static_cast<GLuint>(offset);
				if (a.type == GL_TEXTURE_2D)
				{
					offset	+= sizeof(shader*) * ua.count;
				}
				else
				{
					offset	+= sizeof(float) * ua.count;
				}
				break;
			}
		}
		this->header() 	= offset;
		return	offset;
	}
};

using	vertex_layout	= attribute_layout<vertex_api>;
using	uniform_layout	= attribute_layout<uniform_api>;


AMTRS_OPENGL_NAMESPACE_END
#endif
