/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__types__hpp
#define __libamtrs__filesystem__types__hpp

#if	__has_include(<filesystem>)
#include <filesystem>
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

using	path							= std::filesystem::path;
using	filesystem_error				= std::filesystem::filesystem_error;
using	directory_entry					= std::filesystem::directory_entry;
using	directory_iterator				= std::filesystem::directory_iterator;
using	recursive_directory_iterator	= std::filesystem::recursive_directory_iterator;
using	file_status						= std::filesystem::file_status;
using	space_info						= std::filesystem::space_info;
using	file_type						= std::filesystem::file_type;
using	perms							= std::filesystem::perms;
using	perm_options					= std::filesystem::perm_options;
using	copy_options					= std::filesystem::copy_options;
using	directory_options				= std::filesystem::directory_options;
using	file_time_type					= std::filesystem::file_time_type;

using	path_type	= std::string_view;

AMTRS_FILESYSTEM_NAMESPACE_END
#else
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

using	path_type	= std::string_view;

// ==================================================================
//!	@brief	copy に使用されるオプション。
// ------------------------------------------------------------------
enum class	copy_options
{
	none				= 0,
	skip_existing		= 1,
	overwrite_existing	= 2,
	update_existing		= 3,
	recursive			= 4,
	copy_symlinks		= 5,
	skip_symlinks		= 6,
	directories_only	= 7,
	create_symlinks		= 8,
	create_hard_links	= 9,
};


// ==================================================================
//!	@brief	recursive_directory_iterator で使用される。
// ------------------------------------------------------------------
enum class	directory_options
{
	none					,
	follow_directory_symlink,
	skip_permission_denied	,
};


// ==================================================================
//!	@brief	ファイルの種類
// ------------------------------------------------------------------
enum class	file_type
{
	none		,
	not_found	,
	regular		,
	directory	,
	symlink		,
	block		,
	character	,
	fifo		,
	socket		,
	unknown		,
};


// ==================================================================
//!	@brief	所有権
// ------------------------------------------------------------------
enum class	perms
{
	none			= 0,
	owner_read		= 0400,			// S_IRUSR
	owner_write		= 0200,			// S_IWUSR
	owner_exec		= 0100,			// S_IXUSR
	owner_all		= 0700,			// S_IRWXU
	group_read		= 040,			// S_IRGRP
	group_write		= 020,			// S_IWGRP
	group_exec		= 010,			// S_IXGRP
	group_all		= 070,			// S_IRWXG
	others_read		= 04,			// S_IROTH
	others_write	= 02,			// S_IWOTH
	others_exec		= 01,			// S_IXOTH
	others_all		= 07,			// S_IRWXO
	all				= 0777,
	set_uid			= 04000,		// S_ISUID
	set_gid			= 02000,		// S_ISGID
	sticky_bit		= 01000,		// S_ISVTX
	mask			= 07777,
	unknown			= 0xFFFF
};



// ==================================================================
//!	@brief	ファイルのタイムスタンプ
//!			http://en.cppreference.com/w/cpp/filesystem/file_status
// ------------------------------------------------------------------
using	file_time_type	= std::chrono::time_point<std::chrono::system_clock>;

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



AMTRS_FILESYSTEM_NAMESPACE_END
#endif
#endif
