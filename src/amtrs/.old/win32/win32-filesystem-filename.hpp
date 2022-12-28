/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__filename_win32__hpp
#define __libamtrs__filesystem__filename_win32__hpp
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include "../string.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

static constexpr char	directory_separator[]	= "\\/";





template<class T>
inline auto root_name(T const& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	std::basic_string_view<chartype_t<T>>	retval(_path);
	if (retval.size() >= 2)
	{
		if (std::isalpha(retval[0]) && retval[1] == ':')
		{
			return	retval.substr(0, 2);	
		}
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
