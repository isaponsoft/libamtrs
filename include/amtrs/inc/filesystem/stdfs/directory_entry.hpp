/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
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
