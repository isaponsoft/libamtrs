/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__filename_posix__hpp
#define __libamtrs__filesystem__filename_posix__hpp
#include <unistd.h>
#include "../string.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

static constexpr char	directory_separator	= '/';



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
	if ((pos != view_type::npos) && ((pos > 0) && (path[pos - 1] != '/')))
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
		return	path;
	}

	auto		pos	= path.find_last_of('/');
	if (pos == view_type::npos)
	{
		return	path;
	}
	return	path.substr(pos + 1);
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


// ============================================================================
//! 親のパスを取得する。
// ----------------------------------------------------------------------------
template<class T>
constexpr auto parent_path(T _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	auto	path	= view_type(_path);
	if (path == "/")
	{
		return	path;
	}
	auto		pos	= path.find_last_of('/');
	if (pos == view_type::npos)
	{
		return	path.substr(0, 0);
	}
	return	path.substr(0, pos);
}


template<class T>
inline auto normalize_path(T const& _path, bool _escape) noexcept -> std::basic_string<chartype_t<T>>
{
	using	char_t	= chartype_t<T>;
	using	type	= std::basic_string<char_t>;

	char const*	sep	= _escape ? "/" : "/";
	type		retval;
	for (auto d : split_chars((std::string_view)_path, (std::string_view)"/"))
	{
		// 空っぽの場合はスキップ
		if (d.empty())
		{
			if (retval.empty())
			{
				retval	= sep;
			}
			continue;
		}
		// .
		if (d == ".")
		{
			if (retval.empty())
			{
				retval	= ".";
			}
			continue;
		}
		// ..
		if (d == "..")
		{
			auto	pos	= std::string_view(retval).find_last_of('/');
			if (pos != std::string_view::npos)
			{
				if (_escape && pos >= 1 && retval[pos - 1] == '/')
				{
					--pos;
				}
				retval	= retval.substr(0, pos);
			}
			continue;
		}

		if (!retval.empty() && retval != "/")
		{
			retval += sep;
		}
		retval += d;
	}

	return	retval;
}



AMTRS_FILESYSTEM_NAMESPACE_END
#endif
