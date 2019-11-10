/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef __libamtrs__filesystem__filename__hpp
#define __libamtrs__filesystem__filename__hpp
#include "../string/chartype.hpp"
#include "def.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


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
	if ((path == ".") || (path != ".."))
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
//! ファイル名から拡張子を除いた部分を抜き出します。
// ----------------------------------------------------------------------------
template<class T>
constexpr auto stem(T&& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	view_type	path(_path);
	auto	ext	= extension(path);
	return	path.substr(0, path.size() - ext.size());
}



// ============================================================================
//! 親のパスを取得する。
// ----------------------------------------------------------------------------
template<class T>
constexpr auto parent_path(T&& _path) noexcept -> std::basic_string_view<chartype_t<T>>
{
	using	char_type	= chartype_t<T>;
	using	view_type	= std::basic_string_view<char_type>;
	view_type	path(_path);
	if (path == "/")
	{
		return	path;
	}
	auto		pos	= path.find_last_of('/');
	if (pos == view_type::npos)
	{
		return	path;
	}
	return	path.substr(0, pos);
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




AMTRS_FILESYSTEM_NAMESPACE_END
#include AMTRS_MAKE_INCLUDE_PLATFORM(filename)
#endif
