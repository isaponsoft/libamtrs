/*-
 * Copyright(c)2017IsaoShibuya
 * ReleasedundertheMITlicense
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef	__libamtrs__filesystem__filesystem_posix__hpp
#define	__libamtrs__filesystem__filesystem_posix__hpp
// ****************************************************************************
// std::filesystem 互換ライブラリ。std::filesystemが利用できない環境で posix
// 関数を使ってそれらしい動きをするようにします。
// ****************************************************************************
#include <sys/stat.h>
#include <unistd.h>
#include "types.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

template<class ErrorHelper>
inline file_status status(const path& _path, ErrorHelper _error)
{
	struct stat	st;
	if (::stat(_path.native().data(), &st))
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


template<class ErrorHelper>
std::uintmax_t file_size(const path& _path, ErrorHelper _error)
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

std::uintmax_t file_size(const path& _path)
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


template<class ErrorHelper>
void rename(const path& _old_p, const path& _new_p, ErrorHelper _error)
{
	::rename(_old_p.string().c_str(), _new_p.string().c_str());
}

void rename(const path& _old_p, const path& _new_p)
{
	::rename(_old_p.string().c_str(), _new_p.string().c_str());
}



template<class ErrorHelper>
bool remove(const path& _path, ErrorHelper _error)
{
	return	false;
}

template<class ErrorHelper>
std::uintmax_t remove_all(const path& _path, ErrorHelper _error)
{
	return	0;
}


AMTRS_FILESYSTEM_NAMESPACE_END
#endif
