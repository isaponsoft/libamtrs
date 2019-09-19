/*-
 * Copyright(c)2017IsaoShibuya
 * ReleasedundertheMITlicense
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef	__libamtrs__filesystem__filesystem_posix__hpp
#define	__libamtrs__filesystem__filesystem_posix__hpp
#include <sys/stat.h>
#include <unistd.h>
#include "types.hpp"
AMTRS_NAMESPACE_FILESYSTEM_BEGIN

template<class PathTraits, class ErrorHelper>
inline file_status status(const basic_path<PathTraits>& _path, ErrorHelper _error)
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


template<class PathTraits, class ErrorHelper>
basic_path<PathTraits> current_path(ErrorHelper _error)
{
	char*		name	= getcwd(nullptr, 0);
	if (!name)
	{
		_error(errno, "current_path()");
		return	{};
	}
	basic_path<PathTraits>	retval;
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



template<class PathTraits, class ErrorHelper>
void current_path(const basic_path<PathTraits>& _path, ErrorHelper _error)
{
	int	ret = chdir(_path.c_str());
	if (ret)
	{
		_error(errno, "current_path()");
	}
}

template<class PathTraits, class ErrorHelper>
std::uintmax_t file_size(const basic_path<PathTraits>& _path, ErrorHelper _error)
{
	struct stat	st;
	if (::stat(_path.native().data(), &st) || S_ISDIR (st.st_mode))
	{
		// ファイルが見つからなかった
		_error(errno, "file_size()");
		return	{};
	}

	return	st.st_size;
}



AMTRS_NAMESPACE_FILESYSTEM_END
#endif
