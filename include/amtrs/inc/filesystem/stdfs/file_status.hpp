/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__filesystem__stdfs__file_status__hpp
#define	__libamtrs__filesystem__stdfs__file_status__hpp
#include <chrono>
#include "types.hpp"
AMTRS_FILESYSTEM_STDFS_NAMESPACE_BEGIN


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


AMTRS_FILESYSTEM_STDFS_NAMESPACE_END
#endif
