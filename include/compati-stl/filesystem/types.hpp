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
#ifndef	__libamtrs__filesystem__stdfs__types__hpp
#define	__libamtrs__filesystem__stdfs__types__hpp
#include <chrono>
#include "def.hpp"
AMTRS_STD_FILESYSTEM__NAMESPACE_BEGIN

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


AMTRS_STD_FILESYSTEM__NAMESPACE_END
#endif
