﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__filename__hpp
#define __libamtrs__filesystem__filename__hpp
#include AMTRS_PLATFORM_INCLUDE(filesystem-filename.hpp)
AMTRS_FILESYSTEM_NAMESPACE_BEGIN
template<class C>
bool is_separator(C _c)
{
	return	std::string_view(directory_separator, sizeof(directory_separator)).find_first_of(_c) != std::string_view::npos;
}




//! root_directoryを抜き出す。
//! 抜き出した位置は root_name() 直後の位置であることが保証される。
template<class T>
inline auto root_directory(T const& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	std::basic_string_view<chartype_t<T>>	path(_path);
	if (auto rn = root_name(_path); !rn.empty())
	{
		path	= path.substr(rn.size());
	}
	if (!path.empty() && is_separator(path.front()))
	{
		return	path.substr(0, 1);
	}
	return	path.substr(0, 0);
}


// ============================================================================
//! ルートパスを持っていればルートパスを抜き出します。
// ----------------------------------------------------------------------------
//! OSによってルートパスの解釈は異なります。
// ----------------------------------------------------------------------------
template<class T>
inline auto root_path(T const& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	type	= std::basic_string_view<chartype_t<T>>;
	type	path(_path);
	auto	rn		= root_name(path);
	auto	rd		= root_directory(path);
	if (rn.empty() && rd.empty())
	{
		return	path.substr(0, 0);
	}
	return	path.substr(0, rd.size() + rn.size());
}


template<class T>
constexpr bool is_relative(T const& _path) noexcept
{
	return	root_path(_path).empty() ? true : false;
}


template<class T>
constexpr bool is_absolute(T const& _path) noexcept
{
	return	!is_relative(_path);
}




template<class PathT>
inline std::string absolute_path(PathT _path)
{
	if (is_absolute(_path))
	{
		return	std::string(_path);
	}
	else
	{
		auto	retval	= std::filesystem::current_path().string();
		retval.append(1, directory_separator[0]);
		if (!_path.empty())
		{
			retval.append(_path);
		}
		return	retval;
	}
}



// ============================================================================
//! ディレクトリがある場合、最初のディレクトリを返す
// ----------------------------------------------------------------------------
//!	"foo/bar/"		=> "foo/"
//!	"/foo/bar/"		=> "/"
//!	"foo"			=> ""
// ----------------------------------------------------------------------------
template<class T>
constexpr auto first_dir(T const& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	auto	pos			= _path.find_first_of(directory_separator);
	if (pos == view_type::npos)
	{
		return	_path.substr(0, 0);
	}
	return	_path.substr(0, pos + 1);
}



// ============================================================================
//! 最初のパスを返す
// ----------------------------------------------------------------------------
//!	"foo/bar/"		=> "foo/"
//!	"/foo/bar/"		=> "/"
//!	"foo"			=> "foo"
// ----------------------------------------------------------------------------
template<class T>
constexpr auto first_path(T const& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	auto	pos			= _path.find_first_of(directory_separator);
	if (pos == view_type::npos)
	{
		return	_path;
	}
	return	_path.substr(0, pos + 1);
}






// ============================================================================
//! 最初ににディレクトリセパレータがあれば取り除く
// ----------------------------------------------------------------------------
template<class T>
constexpr auto trim_first_separator(T const& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	view_type	path	= _path;
	while (path.size() >= 1 && is_separator(path.front()))
	{
		path	= path.substr(1);
	}
	return	path;
}



// ============================================================================
//! 最後にディレクトリセパレータがあれば取り除く
// ----------------------------------------------------------------------------
template<class T>
constexpr auto trim_last_separator(T const& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	view_type	path	= _path;
	while (path.size() >= 1 && is_separator(path.back()))
	{
		path	= path.substr(0, path.size() - 1);
	}
	return	path;
}


// ============================================================================
//! 親のパスを抜き出す
// ----------------------------------------------------------------------------
template<class T>
constexpr auto parent_path(T const& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	auto	path	= view_type(_path);
	if (path.empty() || (path.size() == 1 && is_separator(path.front())))
	{
		return	path;
	}
	auto	seppos	= path.find_last_of(directory_separator);
	if (seppos == view_type::npos)
	{
		return	path.substr(0, 0);
	}
	path	= path.substr(0, seppos);
	return	path.empty()
			? path.substr(0, 1)
			: path;
}


// ============================================================================
//! ディレクトリのパスを抜き出す
// ----------------------------------------------------------------------------
//! _levelsでいくつ親を遡るのかを指定できる以外は parent_path() と同じです。
// ----------------------------------------------------------------------------
template<class T>
constexpr auto dirname(T const& _path, size_t _levels) noexcept -> std::basic_string_view<chartype_t<T>>
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



// ============================================================================
//! ファイル名の拡張子に相当する部分を抜き出します。
// ----------------------------------------------------------------------------
template<class T>
constexpr auto extension(T const& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	view_type	path(_path);
	auto		pos1	= path.find_last_of('.');
	if (pos1 == view_type::npos)
	{
		return	path.substr(path.size());
	}
	auto		pos2	= path.find_last_of(directory_separator);
	if (pos2 == view_type::npos || pos1 > pos2)
	{
		return	path.substr(pos1);
	}
	return	path.substr(path.size());
}



// ============================================================================
//! ファイル名からディレクトリを除いた部分を抜き出します。
// ----------------------------------------------------------------------------
template<class T>
constexpr auto filename(T const& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	view_type	path(_path);
	if ((path == ".") || (path == ".."))
	{
		return	path.substr(0, 0);
	}

	auto		pos	= path.find_last_of(directory_separator);
	if (pos == view_type::npos)
	{
		return	path;
	}
	return	path.substr(pos+1);
}


// ============================================================================
//! ファイル名から拡張子を除いた部分を抜き出します。
// ----------------------------------------------------------------------------
template<class T>
constexpr auto stem(T&& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	view_type	path(filename(_path));
	auto	ext	= extension(path);
	return	path.substr(0, path.size() - ext.size());
}





template<class T, class Path, class Sep>
inline auto replace_path_separator(Path _path, Sep _separator) noexcept -> T
{
	using	char_t	= chartype_t<T>;
	using	type	= T;

	bool				fstSep	= false;
	bool				lstSep	= false;
	std::string_view	path(_path);
	if (!path.empty())
	{
		if (is_separator(path.front()))
		{
			fstSep	= true;
			path	= path.substr(1);
			if (!path.empty())
			{
				if (is_separator(path.back()))
				{
					lstSep	= true;
					path	= path.substr(0, path.size() - 1);
				}
			}
		}
	}
	type							retval;
	if (fstSep)
	{
		retval += (_separator);
	}

	bool	first	= true;
	for (auto dir : split_chars(path, std::string_view(directory_separator)))
	{
		// 空っぽの場合はスキップ
		if (dir.empty())
		{
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
			auto	pos	= std::string_view(retval).find_last_of(directory_separator);
			if (pos != std::string_view::npos)
			{
				retval	= retval.substr(0, pos);
			}
			continue;
		}

		if (!first)
		{
			retval += (_separator);
		}
		retval += dir;
		first	= false;
	}
	if (lstSep)
	{
		retval += (_separator);
	}
	return	retval;
}


template<class Path>
inline auto to_slashpath(Path const& _path) -> std::basic_string<chartype_t<Path>>
{
	return	replace_path_separator<std::basic_string<chartype_t<Path>>>(_path, "/");
}



// ============================================================================
//! ファイル名を正規化します。
// ----------------------------------------------------------------------------
//! // が / になります。また、Windowsでは / が \ に置き換わります。
//!	_escape
//!		0以外を指定した場合、指定した文字でセパレータをエスケープします。
// ----------------------------------------------------------------------------
template<class Path>
auto normalize_path(Path const& _path, chartype_t<Path> _escape = 0) noexcept -> std::basic_string<chartype_t<Path>>
{
	if (_escape)
	{
		char	sep[] = { _escape, directory_separator[0], '\0' };
		return	replace_path_separator<std::basic_string<chartype_t<Path>>>(_path, sep);
	}
	else
	{
		return	replace_path_separator<std::basic_string<chartype_t<Path>>>(_path, directory_separator[0]);
	}
}



// ============================================================================
//! 一致している親パスを調べてその長さを返す。
// ----------------------------------------------------------------------------
//!	match_parent_path("foo/bar/to", "foo/bar/bar") -> 8 ... "foo/bar/"
// ----------------------------------------------------------------------------
template<class A, class B>
size_t match_parent_path(A&& _a, B&& _b)
{
	std::string_view	a(_a);
	std::string_view	b(_b);
	std::string_view	r(a.substr(0, 0));
	for (;;)
	{
		auto	d	= first_path(a);
		if (d.empty())
		{
			break;
		}
		if (!starts_with(b, d))
		{
			break;
		}
		a	= a.substr(d.size());
		b	= b.substr(d.size());
		r	= std::string_view(r.data(), (size_t)((d.data() + d.size()) - r.data()));
	}
	return	r.size();
}



// ============================================================================
//! イテレータで指定するパスから共通しているパスの長さを返す。
// ----------------------------------------------------------------------------
//!	paths[] = { "foo/bar/to", "foo/foo", "foo/bar/bar" }
//!	common_parent_path(begin(paths), end(paths)) -> 4 ... "foo/"
// ----------------------------------------------------------------------------
template<class It>
auto common_parent_path(It _first, It _last) -> size_t
{
	std::string_view	r(nullptr, 0);
	if (_first != _last)
	{
		r	= parent_path(std::string_view(*_first));
		++_first;
		while (_first != _last)
		{
			auto	m	= match_parent_path(r, std::string_view(*_first));
			r	= r.substr(0, m);
			if (m == 0)
			{
				break;
			}
			++_first;
		}
	}
	return	r.size();
}


// ============================================================================
//! ルートパスを持っていればルートパスを除外します。
// ----------------------------------------------------------------------------
//! OSによってルートパスの解釈は異なります。
// ----------------------------------------------------------------------------
template<class T>
inline auto trim_root_path(T const& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	auto	rp	= root_path(_path);
	return	_path.substr(rp.size());
}


AMTRS_FILESYSTEM_NAMESPACE_END


#endif
