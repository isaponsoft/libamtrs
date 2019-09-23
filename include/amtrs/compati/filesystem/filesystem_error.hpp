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
#ifndef	__libamtrs__filesystem__filesystem_error__hpp
#define	__libamtrs__filesystem__filesystem_error__hpp
#include <string>
#include <cstring>
#include "types.hpp"
#include "path.hpp"
AMTRS_FILESYSTEM_STDFS_NAMESPACE_BEGIN


// ==================================================================
//!	@brief	エラーを示すクラス。
//!			http://en.cppreference.com/w/cpp/filesystem/file_status
// ------------------------------------------------------------------
template<class PathT>
class	basic_filesystem_error
		: public std::system_error
{
protected:
		using	path_type	= PathT;

public:

	basic_filesystem_error(const std::string& _what_tag, std::error_code _ec)
		: std::system_error(_ec, _what_tag)
	{
	}

	basic_filesystem_error(const std::string& _what_tag, const path_type& _path1, std::error_code _ec)
		: std::system_error(_ec, _what_tag)
		, m_path1(_path1)
	{
	}

	basic_filesystem_error(const std::string& _what_tag, const path_type& _path1, const path_type& _path2, std::error_code _ec)
		: std::system_error(_ec, _what_tag)
		, m_path1(_path1)
		, m_path2(_path2)
	{
	}

	~basic_filesystem_error(){}

	const path_type path1() const { return m_path1; }
	const path_type path2() const { return m_path2; }

private:
	path_type				m_path1;
	path_type				m_path2;
};


// ==================================================================
//!	@brief	エラーハンドリングを書きやすくするためのヘルパ
// ------------------------------------------------------------------
namespace error
{
	// ==================================================================
	//!	@brief	エラーが起きたときに、コンストラクタで指定される
	//!			std::error_code の参照先に値をセットし、例外を投げません。
	// ------------------------------------------------------------------
	template<class PathT>	// = std::basic_filesystem_error<>
	class	set_error
	{
	public:
		using	error_type	= std::error_code;
		using	throw_type	= basic_filesystem_error<PathT>;
		set_error(error_type& _ec) : m_ec(_ec) { m_ec.clear(); }
		template<class... Argv> void operator () (int _errorno, const char*, Argv&&...) noexcept
		{
			m_ec = error_type(_errorno, std::generic_category());
		}
		void operator = (int _errorno) noexcept
		{
			m_ec = error_type(_errorno, std::generic_category());
		}
		template<class... Argv> void throw_if(const char*, Argv&&... _argv) const noexcept {}
	private:
		error_type&	m_ec;
	};


	// ==================================================================
	//!	@brief	エラーが起きたときに、std::filesystem::filesystem_error
	//!			を投げます。
	// ------------------------------------------------------------------
	template<class PathT>	// = std::basic_filesystem_error<>
	class	throw_error
	{
	public:
		using	error_type	= std::error_code;
		using	throw_type	= basic_filesystem_error<PathT>;
		throw_error() : m_error(0) {}
		template<class... Argv> void operator () (int _errorno, const char* _msg, Argv&&... _argv) const
		{
			std::string	msg("amtrs::filesystem::");
			msg += _msg;
			throw	throw_type(std::move(msg), std::forward<Argv>(_argv)..., error_type(_errorno, std::generic_category()));
		}
		void operator = (int _errorno) noexcept
		{
			m_error = _errorno;
		}
		template<class... Argv> void throw_if(const char* _msg, Argv&&... _argv) const
		{
			if (m_error)
			{
				std::string	msg("amtrs::filesystem::");
				msg += _msg;
				throw	throw_type(std::move(msg), std::forward<Argv>(_argv)..., error_type(m_error, std::generic_category()));
			}
		}
	private:
		int	m_error;
	};

	template<class PathT>
	auto make(std::error_code& _ec) -> set_error<PathT>
	{
		return	{_ec};
	}

	template<class PathT>
	auto make() -> throw_error<PathT>
	{
		return	{};
	}
}

AMTRS_FILESYSTEM_STDFS_NAMESPACE_END
#endif
