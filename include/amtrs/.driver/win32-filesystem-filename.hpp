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

static constexpr char	directory_separator	= '\\';



// ============================================================================
//! ファイル名の拡張子に相当する部分を抜き出します。
// ----------------------------------------------------------------------------
template<class T>
constexpr auto extension(T&& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	view_type	path(_path);
	auto		pos	= path.find_last_of('.');
	if ((pos != view_type::npos) && ((pos > 0) && (path[pos - 1] != '/' || path[pos - 1] != '\\')))
	{
		path	= path.substr(pos);
	}
	return	path;
}



// ============================================================================
//! ファイル名からディレクトリを除いた部分を抜き出します。
// ----------------------------------------------------------------------------
template<class T>
constexpr auto filename(T&& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	view_type	path(_path);
	if ((path == ".") || (path == ".."))
	{
		return	path.substr(0, 0);
	}

	auto		pos1	= path.find_last_of('/');
	auto		pos2	= path.find_last_of('\\');
	if (pos1 == view_type::npos && pos2 == view_type::npos)
	{
		return	path;
	}
	if (pos1 == view_type::npos)
	{
		return	path.substr(pos2+1);
	}
	if (pos2 == view_type::npos)
	{
		return	path.substr(pos1+1);
	}
	return	path.substr(std::max(pos1, pos2)+1);
}





// ============================================================================
//! ディレクトリのパスを取得する。
// ----------------------------------------------------------------------------
//! _levelsでいくつ親を遡るのかを指定できる以外は parent_path() と同じです。
// ----------------------------------------------------------------------------
template<class T>
constexpr auto dirname(T&& _path, size_t _levels) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;

	view_type	path(_path);
	for (; _levels > 0; --_levels)
	{
		path	= parent_path(path);
	}
	return	path;
}

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
//! 親のパスを取得する。
// ----------------------------------------------------------------------------
template<class T>
constexpr auto parent_path(T&& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	auto	path	= view_type(_path);
	if (path == "/" || path == "\\")
	{
		return	path;
	}
	auto		pos1	= path.find_last_of('/');
	auto		pos2	= path.find_last_of('\\');
	if (pos1 == view_type::npos && pos2 == view_type::npos)
	{
		return	path.substr(0, 0);
	}
	if (pos1 == view_type::npos)
	{
		return	path.substr(0, pos2);
	}
	if (pos2 == view_type::npos)
	{
		return	path.substr(0, pos1);
	}
	return	path.substr(0, std::max(pos1, pos2));
}


template<class T>
inline auto normalize_path(T const& _path, bool _escape) noexcept -> std::basic_string<chartype_t<T>>
{
	using	char_t	= chartype_t<T>;
	using	type	= std::basic_string<char_t>;

	char const*	sep	= _escape ? "\\\\" : "\\";
	type		retval;
	for (auto dir : split_chars((std::string_view)_path, (std::string_view)"/\\"))
	{
		// 空っぽの場合はスキップ
		if (dir.empty())
		{
			if (retval.empty())
			{
				retval	= sep;
			}
			continue;
		}
		// .
		if (dir == ".")
		{
			if (retval.empty())
			{
				retval	= ".";
			}
			continue;
		}
		// ..
		if (dir == "..")
		{
			auto	pos	= std::string_view(retval).find_last_of('\\');
			if (pos != std::string_view::npos)
			{
				if (_escape && pos >= 1 && retval[pos - 1] == '\\')
				{
					--pos;
				}
				retval	= retval.substr(0, pos);
			}
			continue;
		}

		if (!retval.empty())
		{
			retval += sep;
		}
		retval += dir;
	}


#if	0
	size_t	sz		= 0;
	auto	s		= std::basic_string_view<char_t>(_path);
	for (auto it = s.begin(); it != s.end(); )
	{
		++sz;
		if (*it == '/' || *it == '\\')
		{
			if (_escape)
			{
				++sz;
			}
			do
			{
				++it;
			} while ((it != s.end()) && (*it == '/' || *it == '\\'));
			continue;
		}
		++it;
	}

	type	retval;
	retval.resize(sz);
	auto*	d	= retval.data();
	for (auto it = s.begin(); it != s.end(); )
	{
		if (*it == '/' || *it == '\\')
		{
			*d++	= '\\';
			if (_escape)
			{
				*d++	= '\\';
			}
			do
			{
				++it;
			} while ((it != s.end()) && (*it == '/' || *it == '\\'));
			continue;
		}
		*d++	= *it;
		++it;
	}
#endif
	return	retval;
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
