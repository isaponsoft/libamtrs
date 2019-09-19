﻿/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__filesystem__vfs__hpp
#define __libamtrs__filesystem__vfs__hpp
#include <iostream>
#include <fstream>
#include <system_error>
#include "../../logging.hpp"
#include "../../iostream.hpp"
#include "def.hpp"
#include "types.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

// ============================================================================
//! 仮想ファイルシステムのインターフェース
// ----------------------------------------------------------------------------
//! 仮想ファイルシステムは、std::filesystem だけではなく、
//! アーカイブファイルなどの異なるストレージへのいアクセスを抽象化します。
// ----------------------------------------------------------------------------
class	vfs : public ref_object
{
	template<class Call>
	static auto throwmode(const path& _path, Call&& _call)
	{
		std::error_code ec;
		auto			retval	= _call(ec);
		if (ec)
		{
			std::system_error	se(ec, _path.string());
			AMTRS_DEBUG_LOG("%s", se.what());
			throw	se;
		}
		return	retval;
	}

public:
	ios::iovstream open(const path& _path, std::error_code& _ec) {                                                     return on_open(_path, _ec);     }
	ios::iovstream open(const path& _path                      ) { return throwmode(_path, [&](std::error_code& _ec) { return on_open(_path, _ec); }); }


	file_status		status		(const path& _path, std::error_code& _ec) const {                                              return on_status   (_path, _ec);     }
	file_status		status		(const path& _path                      ) const { return throwmode(_path, [&](std::error_code& _ec) { return on_status   (_path, _ec); }); }

	std::uintmax_t	file_size	(const path& _path, std::error_code& _ec) const {                                              return on_file_size(_path, _ec);     }
	std::uintmax_t	file_size	(const path& _path                      ) const { return throwmode(_path, [&](std::error_code& _ec) { return on_file_size(_path, _ec); }); }


	bool			remove		(const path& _path, std::error_code& _ec) const {                                              return on_remove	   (_path, _ec);     }
	bool			remove		(const path& _path                      ) const { return throwmode(_path, [&](std::error_code& _ec) { return on_remove	   (_path, _ec); }); }
	std::uintmax_t	remove_all	(const path& _path, std::error_code& _ec) const {                                              return on_remove_all(_path, _ec);     }
	std::uintmax_t	remove_all	(const path& _path                      ) const { return throwmode(_path, [&](std::error_code& _ec) { return on_remove_all(_path, _ec); }); }

	// status(path) に対するユーティリティメソッド群
	#define	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(_name)	\
	bool _name(const path& _path, std::error_code& _ec) const {                                              return stdfs::_name(status(_path, _ec));     }\
	bool _name(const path& _path                      ) const { return throwmode(_path, [&](std::error_code& _ec) { return stdfs::_name(status(_path, _ec)); }); }

	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(exists)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_block_file)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_character_file)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_directory)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_fifo)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_regular_file)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_socket)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_symlink)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_other)

	#undef	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD

protected:
	virtual ios::iovstream on_open      (const path& _path, std::error_code& _ec)       = 0;
	virtual file_status    on_status    (const path& _path, std::error_code& _ec) const = 0;
	virtual std::uintmax_t on_file_size (const path& _path, std::error_code& _ec) const = 0;
	virtual bool		   on_remove    (const path& _path, std::error_code& _ec) const = 0;
	virtual std::uintmax_t on_remove_all(const path& _path, std::error_code& _ec) const = 0;
};




AMTRS_FILESYSTEM_NAMESPACE_END
#endif
