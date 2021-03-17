/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__filename_posix__hpp
#define __libamtrs__filesystem__filename_posix__hpp
#include <unistd.h>
#include "../string.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

static constexpr char	directory_separator[]	= "/";



template<class T>
inline auto root_name(T const& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	auto	retval	= std::basic_string_view<chartype_t<T>>(_path).substr(0, 1);
	if (retval == "/")
	{
		return	retval;
	}
	return	retval.substr(0, 0);
}


// ============================================================================
//! Tで指定する型に現在のカレントディレクトリを格納して返す。
// ----------------------------------------------------------------------------
template<class T>
inline T current_path()
{
	T			retval;
	if (char* name = getcwd(nullptr, 0); name)
	{
		try
		{
			retval	= decltype(retval)(name);
		}
		catch (...)
		{
			free(name);
			throw;
		}
		free(name);
	}
	return	retval;
}



AMTRS_FILESYSTEM_NAMESPACE_END
#endif
