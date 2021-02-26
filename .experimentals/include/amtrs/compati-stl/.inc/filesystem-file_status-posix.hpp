/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__filesystem__stdfs__function_posix__hpp
#define	__libamtrs__filesystem__stdfs__function_posix__hpp
// ****************************************************************************
// std::filesystem 互換ライブラリ。std::filesystemが利用できない環境で posix
// 関数を使ってそれらしい動きをするようにします。
// ****************************************************************************
#include <sys/stat.h>
#include <unistd.h>
#include "types.hpp"
#include "path.hpp"
AMTRS_STD_FILESYSTEM__NAMESPACE_BEGIN

class	filesystem_error
		: public std::system_error
{
public:
	filesystem_error(const std::string& _what, std::error_code _ec)
		: system_error(_ec, _what)
	{}

	filesystem_error(const std::string& _what, const path& _path1, std::error_code _ec)
		: system_error(_ec, _what)
		, mPath1(_path1)
	{}

	filesystem_error(const std::string& _what, const path& _path1, const path& _path2, std::error_code _ec)
		: system_error(_ec, _what)
		, mPath1(_path1)
		, mPath2(_path2)
	{}

	const path& path1() const noexcept { return mPath1; }
	const path& path2() const noexcept { return mPath2; }

private:
	path	mPath1;
	path	mPath2;
};



inline file_status status(const path& _path, std::error_code& _ec)
{
	_ec.clear();

	struct stat	st;
	if (::stat(_path.native().c_str(), &st))
	{
		// ファイルが見つからなかった
		return	file_status(file_type::not_found);
	}

	// file type.
	file_status		retval;
	retval.type
	(
		S_ISREG (st.st_mode) ? file_type::regular   :
		S_ISDIR (st.st_mode) ? file_type::directory :
		S_ISLNK (st.st_mode) ? file_type::symlink   :
		S_ISBLK (st.st_mode) ? file_type::block     :
		S_ISCHR (st.st_mode) ? file_type::character :
		S_ISFIFO(st.st_mode) ? file_type::fifo      :
		S_ISSOCK(st.st_mode) ? file_type::socket    :
		               file_type::unknown
	);
	return	retval;
}


inline file_status status(const path& _path)
{
	std::error_code	e;
	auto			r	= status(_path, e);
	if (e)
	{
		throw	filesystem_error(e.message(), e);
	}
	return	r;
}



inline path current_path(std::error_code& _ec)
{
	_ec.clear();
	char*		name	= getcwd(nullptr, 0);
	if (!name)
	{
		return	{};
	}
	path	retval;
	try
	{
		retval	= decltype(retval){name};
	}
	catch (...)
	{
		free(name);
		throw;
	}
	free(name);
	return	retval;
}



inline path current_path()
{
	std::error_code	ec;
	return	current_path(ec);
}


inline bool exists(const file_status& _status)
{
	return	_status.type() != file_type::not_found;
}


inline bool exists(const path& _path, std::error_code& _ec)
{
	return	exists(status(_path, _ec));
}


inline bool exists(const path& _path)
{
	return	exists(status(_path));
}


inline bool is_block_file(const file_status& _status)
{
	return	false;
}


inline bool is_block_file(const path& _path, std::error_code& _ec)
{
	return	false;
}

inline bool is_block_file(const path& _path)
{
	return	false;
}


inline bool is_character_file(const file_status& _status)
{
	return	false;
}


inline bool is_character_file(const path& _path, std::error_code& _ec)
{
	return	false;
}

inline bool is_character_file(const path& _path)
{
	return	false;
}


inline bool is_directory(const file_status& _status)
{
	return	_status.type() == file_type::directory;
}


inline bool is_directory(const path& _path, std::error_code& _ec)
{
	return	is_directory(status(_path));
}

inline bool is_directory(const path& _path)
{
	return	is_directory(status(_path));
}


inline bool is_fifo(const file_status& _status)
{
	return	_status.type() == file_type::fifo;
}

inline bool is_fifo(const path& _path, std::error_code& _ec)
{
	return	is_fifo(status(_path));
}

inline bool is_fifo(const path& _path)
{
	return	is_fifo(status(_path));
}


inline bool is_regular_file(const file_status& _status)
{
	return	_status.type() == file_type::regular;
}

inline bool is_regular_file(const path& _path, std::error_code& _ec)
{
	return	is_regular_file(status(_path));
}

inline bool is_regular_file(const path& _path)
{
	return	is_regular_file(status(_path));
}


inline bool is_socket(const file_status& _status)
{
	return	false;
}

inline bool is_socket(const path& _path, std::error_code& _ec)
{
	return	false;
}

inline bool is_socket(const path& _path)
{
	return	false;
}


inline bool is_symlink(const file_status& _status)
{
	return	false;
}

inline bool is_symlink(const path& _path, std::error_code& _ec)
{
	return	false;
}

inline bool is_symlink(const path& _path)
{
	return	false;
}


inline bool is_other(const file_status& _status)
{
	return	false;
}

inline bool is_other(const path& _path, std::error_code& _ec)
{
	return	false;
}

inline bool is_other(const path& _path)
{
	return	false;
}


inline std::uintmax_t file_size(const file_status& _status)
{
	return	false;
}


inline std::uintmax_t file_size(const path& _path, std::error_code& _ec)
{
	struct stat	st;
	if (::stat(_path.native().data(), &st) || S_ISDIR (st.st_mode))
	{
		// ファイルが見つからなかった
//		_error(errno, "file_size()");
		return	{};
	}

	return	st.st_size;
}

inline std::uintmax_t file_size(const path& _path)
{
	std::error_code	ec;
	return	file_size(_path, ec);
}


inline void rename(const path& _old_p, const path& _new_p, std::error_code& _ec)
{
	::rename(_old_p.native().c_str(), _new_p.native().c_str());
}


inline void rename(const path& _old_p, const path& _new_p)
{
	::rename(_old_p.native().c_str(), _new_p.native().c_str());
}


inline bool remove(const path& _path, std::error_code& _ec)
{
	_ec.clear();
	auto	r	= ::remove(_path.native().c_str());
	return	r == 0;
}


inline bool remove(const path& _path)
{
	std::error_code	e;
	auto			r	= remove(_path, e);
	if (e)
	{
		throw	filesystem_error(e.message(), e);
	}
	return	r;
}


inline std::uintmax_t remove_all(const path& _path, std::error_code& _ec)
{
	return	0;
}


inline std::uintmax_t remove_all(const path& _path)
{
	return	0;
}


AMTRS_STD_FILESYSTEM__NAMESPACE_END
#endif
