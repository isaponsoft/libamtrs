/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__filesystem__stdfs__file_status__hpp
#define	__libamtrs__filesystem__stdfs__file_status__hpp
#include <chrono>
#include "path.hpp"
AMTRS_STD_FILESYSTEM__NAMESPACE_BEGIN


// ==================================================================
//!	@brief	ファイルの状態
//!			http://en.cppreference.com/w/cpp/filesystem/file_status
// ------------------------------------------------------------------
class	file_status
{
public:
	file_status() : file_status(file_type::none) { }
	file_status(const file_status& ) = default;
	file_status(      file_status&&) = default;
	explicit file_status(file_type _type, perms _permissions = perms::unknown) : m_type(_type), m_perms(_permissions) { }
	~file_status() = default;
	file_status& operator = (const file_status& ) = default;
	file_status& operator = (      file_status&&) = default;

	file_type type() const { return m_type; }
	void type(file_type _type) { m_type = _type; }

	perms permissions() const { return m_perms; }
	void permissions(perms _perms) { m_perms = _perms; }

protected:
	file_type	m_type;
	perms		m_perms;
};


AMTRS_STD_FILESYSTEM__NAMESPACE_END
#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#include "file_status-win32.hpp"
#else
#include "file_status-posix.hpp"
#endif
#endif
