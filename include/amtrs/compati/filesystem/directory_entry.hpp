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
#ifndef	__libamtrs__filesystem__directory_entry__hpp
#define	__libamtrs__filesystem__directory_entry__hpp
#include <cstdint>
#include <memory>
#include "types.hpp"
#include "path.hpp"
#include "file_status.hpp"
AMTRS_FILESYSTEM_STDFS_NAMESPACE_BEGIN

template<class PathT>
class	basic_directory_entry;

using		directory_entry					= basic_directory_entry<path>;


// ==================================================================
//!	@brief	ディレクトリエントリ。
//!			http://en.cppreference.com/w/cpp/filesystem/basic_directory_entry
// ------------------------------------------------------------------
template<class PathT>
class	basic_directory_entry
{
public:
	using	this_type			= basic_directory_entry;
	using	path_type			= PathT;

	basic_directory_entry() noexcept = default;
	basic_directory_entry(const basic_directory_entry& ) = default;
	basic_directory_entry(      basic_directory_entry&&) noexcept = default;
	basic_directory_entry& operator = (const basic_directory_entry& _other) = default;
	basic_directory_entry& operator = (basic_directory_entry&& _other) = default;

	explicit basic_directory_entry(const path_type& _path, std::error_code& _error_code);


	void assign(const path_type& _path) { m_path = _path; }
	void assign(const path_type& _path, std::error_code& _ec) { m_path = _path; }

	const path_type& path() const noexcept { return m_path; }
	operator const path_type& () const noexcept { return m_path; }

	void refresh() const;
	void refresh(std::error_code& _ec) noexcept;

	bool exists() const { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::exists(m_path); }
	bool exists(std::error_code& _ec) const noexcept  { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::exists(m_path, _ec); }

	file_status status() const { return status(m_path); }
	file_status status(std::error_code& _ec) const noexcept { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::status(m_path, _ec); }

	
	bool is_block           (                    ) const          { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_block_file(m_path); }
	bool is_block           (std::error_code& _ec) const noexcept { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_block_file(m_path, _ec); }
	bool is_character_file  (                    ) const          { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_character_file(m_path); }
	bool is_character_file  (std::error_code& _ec) const noexcept { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_character_file(m_path, _ec); }
	bool is_directory       (                    ) const          { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_directory(m_path); }
	bool is_directory       (std::error_code& _ec) const noexcept { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_directory(m_path, _ec); }
	bool is_fifo            (                    ) const          { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_fifo(m_path); }
	bool is_fifo            (std::error_code& _ec) const noexcept { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_fifo(m_path, _ec); }
	bool is_other           (                    ) const          { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_other(m_path); }
	bool is_other           (std::error_code& _ec) const noexcept { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_other(m_path, _ec); }
	bool is_regular_file    (                    ) const          { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_regular_file(m_path); }
	bool is_regular_file    (std::error_code& _ec) const noexcept { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_regular_file(m_path, _ec); }
	bool is_socket          (                    ) const          { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_socket(m_path); }
	bool is_socket          (std::error_code& _ec) const noexcept { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_socket(m_path, _ec); }
	bool is_symlink         (                    ) const          { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_symlink(m_path); }
	bool is_symlink         (std::error_code& _ec) const noexcept { return AMTRS_FILESYSTEM_STDFS_NAMESPACE::is_symlink(m_path, _ec); }


	bool operator == (const basic_directory_entry& _rhs) const noexcept { return m_path == _rhs.m_path; }
	bool operator != (const basic_directory_entry& _rhs) const noexcept { return m_path != _rhs.m_path; }
	bool operator <  (const basic_directory_entry& _rhs) const noexcept { return m_path <  _rhs.m_path; }
	bool operator <= (const basic_directory_entry& _rhs) const noexcept { return m_path <= _rhs.m_path; }
	bool operator >  (const basic_directory_entry& _rhs) const noexcept { return m_path >  _rhs.m_path; }
	bool operator >= (const basic_directory_entry& _rhs) const noexcept { return m_path >= _rhs.m_path; }

private:
	path_type						m_path;
};

AMTRS_FILESYSTEM_STDFS_NAMESPACE_END
#endif
