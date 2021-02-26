/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__vfs__hpp
#define __libamtrs__filesystem__vfs__hpp
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
	static auto throwmode(path_type _path, Call&& _call)
	{
		std::error_code ec;
		auto			retval	= _call(ec);
		if (ec)
		{
			std::system_error	se(ec, (std::string)_path);
			AMTRS_DEBUG_LOG("%s", se.what());
			throw	se;
		}
		return	retval;
	}

	template<class Call>
	static auto throwmode_pass_exists(path_type _path, Call&& _call)
	{
		std::error_code ec;
		auto			retval	= _call(ec);
		if (ec && ec != std::errc::no_such_file_or_directory)
		{
			std::system_error	se(ec, (std::string)_path);
			AMTRS_DEBUG_LOG("%s", se.what());
			throw	se;
		}
		return	retval;
	}

public:
	io::iovstream open(path_type _path, std::error_code& _ec) {                                                     return on_open(_path, _ec);     }
	io::iovstream open(path_type _path                      ) { return throwmode(_path, [&](std::error_code& _ec) { return on_open(_path, _ec); }); }


	file_status		status		(path_type _path, std::error_code& _ec) const {                                              return on_status   (_path, _ec);     }
	file_status		status		(path_type _path                      ) const { return throwmode_pass_exists(_path, [&](std::error_code& _ec) { return on_status   (_path, _ec); }); }

	std::uintmax_t	file_size	(path_type _path, std::error_code& _ec) const {                                              return on_file_size(_path, _ec);     }
	std::uintmax_t	file_size	(path_type _path                      ) const { return throwmode_pass_exists(_path, [&](std::error_code& _ec) { return on_file_size(_path, _ec); }); }


	bool			remove		(path_type _path, std::error_code& _ec) const {                                              return on_remove	   (_path, _ec);     }
	bool			remove		(path_type _path                      ) const { return throwmode(_path, [&](std::error_code& _ec) { return on_remove	   (_path, _ec); }); }
	std::uintmax_t	remove_all	(path_type _path, std::error_code& _ec) const {                                              return on_remove_all(_path, _ec);     }
	std::uintmax_t	remove_all	(path_type _path                      ) const { return throwmode(_path, [&](std::error_code& _ec) { return on_remove_all(_path, _ec); }); }



	inline bool exists(const file_status& _status)
	{
		return	_status.type() != file_type::not_found;
	}
	
	
	inline bool exists(path_type _path, std::error_code& _ec)
	{
		return	exists(status(_path, _ec));
	}
	
	
	inline bool exists(path_type _path)
	{
		std::error_code	ec;
		return	exists(status(_path, ec));
	}
	
	
	inline bool is_block_file(const file_status& _status)
	{
		return	false;
	}
	
	
	inline bool is_block_file(path_type _path, std::error_code& _ec)
	{
		return	false;
	}
	
	inline bool is_block_file(path_type _path)
	{
		return	false;
	}
	
	
	inline bool is_character_file(const file_status& _status)
	{
		return	false;
	}
	
	
	inline bool is_character_file(path_type _path, std::error_code& _ec)
	{
		return	false;
	}
	
	inline bool is_character_file(path_type _path)
	{
		return	false;
	}
	
	
	inline bool is_directory(const file_status& _status)
	{
		return	_status.type() == file_type::directory;
	}
	
	
	inline bool is_directory(path_type _path, std::error_code& _ec)
	{
		return	is_directory(status(_path));
	}
	
	inline bool is_directory(path_type _path)
	{
		return	is_directory(status(_path));
	}
	
	
	inline bool is_fifo(const file_status& _status)
	{
		return	_status.type() == file_type::fifo;
	}
	
	inline bool is_fifo(path_type _path, std::error_code& _ec)
	{
		return	is_fifo(status(_path));
	}
	
	inline bool is_fifo(path_type _path)
	{
		return	is_fifo(status(_path));
	}
	
	
	inline bool is_regular_file(const file_status& _status)
	{
		return	_status.type() == file_type::regular;
	}
	
	inline bool is_regular_file(path_type _path, std::error_code& _ec)
	{
		return	is_regular_file(status(_path));
	}
	
	inline bool is_regular_file(path_type _path)
	{
		return	is_regular_file(status(_path));
	}
	
	
	inline bool is_socket(const file_status& _status)
	{
		return	false;
	}
	
	inline bool is_socket(path_type _path, std::error_code& _ec)
	{
		return	false;
	}
	
	inline bool is_socket(path_type _path)
	{
		return	false;
	}
	
	
	inline bool is_symlink(const file_status& _status)
	{
		return	false;
	}
	
	inline bool is_symlink(path_type _path, std::error_code& _ec)
	{
		return	false;
	}
	
	inline bool is_symlink(path_type _path)
	{
		return	false;
	}
	
	
	inline bool is_other(const file_status& _status)
	{
		return	false;
	}
	
	inline bool is_other(path_type _path, std::error_code& _ec)
	{
		return	false;
	}
	
	inline bool is_other(path_type _path)
	{
		return	false;
	}
	

protected:
	virtual io::iovstream      on_open      (path_type _path, std::error_code& _ec)       = 0;
	virtual file_status    on_status    (path_type _path, std::error_code& _ec) const = 0;
	virtual std::uintmax_t on_file_size (path_type _path, std::error_code& _ec) const = 0;
	virtual bool		   on_remove    (path_type _path, std::error_code& _ec) const = 0;
	virtual std::uintmax_t on_remove_all(path_type _path, std::error_code& _ec) const = 0;
};




AMTRS_FILESYSTEM_NAMESPACE_END
#endif
