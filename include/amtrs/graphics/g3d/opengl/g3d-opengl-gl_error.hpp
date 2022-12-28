/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__opengl__gl_error__hpp
#define	__libamtrs__graphics__opengl__gl_error__hpp
#include "g3d-opengl-def.hpp"
AMTRS_OPENGL_NAMESPACE_BEGIN


#if		AMTRS_DEBUG_LOG
#define	AMTRS_G3D_OPENGL_GLGETERROR(msg)	\
		if (auto err = glGetError()) \
		{\
			auto	m	= amtrs::format<std::string>("%s %s", msg, amtrs::g3d::opengl::get_error_string(err));\
			AMTRS_DEBUG_LOG("%s", m.c_str());\
			throw std::runtime_error(m);\
		}
#else
#define	AMTRS_G3D_OPENGL_GLGETERROR(...)
#endif


inline const char* get_error_string(GLenum _gl_error)
{
#	define	____MSG(_name)	case _name: { return #_name; }
	switch (_gl_error)
	{
		____MSG(GL_NO_ERROR)
		____MSG(GL_INVALID_ENUM)
		____MSG(GL_INVALID_VALUE)
		____MSG(GL_INVALID_OPERATION)
		____MSG(GL_OUT_OF_MEMORY)
		____MSG(GL_INVALID_FRAMEBUFFER_OPERATION)

#ifdef GL_STACK_OVERFLOW
		____MSG(GL_STACK_OVERFLOW)
#endif
#ifdef GL_STACK_UNDERFLOW
		____MSG(GL_STACK_UNDERFLOW)
#endif
#ifdef GL_TABLE_TOO_LARGE
		____MSG(GL_TABLE_TOO_LARGE)
#endif

		default:
			return	"Unkown";
	}
#	undef	____MSG
}


inline void check_and_exception(const std::string& _msg)
{
	auto	e = glGetError();
	if (e)
	{
		std::string	msg	= get_error_string(e);
		throw std::runtime_error(msg + ":" + _msg);
	}
}


AMTRS_OPENGL_NAMESPACE_END
#endif
