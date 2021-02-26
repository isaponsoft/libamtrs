/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__filename__hpp
#define __libamtrs__filesystem__filename__hpp
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


// ============================================================================
//! ルートパスを持っていればルートパスを抜き出します。
// ----------------------------------------------------------------------------
//! OSによってルートパスの解釈は異なります。
// ----------------------------------------------------------------------------
template<class T>
inline auto root_name(T const& _path) noexcept -> std::basic_string_view<chartype_t<T>>;


// ============================================================================
//! ファイル名を正規化します。
// ----------------------------------------------------------------------------
//! // が / になります。また、Windowsでは / が \ に置き換わります。
//!	_escape
//!		true の時、バックスラッシュをエスケープします。
// ----------------------------------------------------------------------------
template<class T>
inline auto normalize_path(T const& _path, bool _escape = false) noexcept -> std::basic_string<chartype_t<T>>;




template<class T>
constexpr bool is_relative(T const& _path) noexcept
{
	return	root_name(_path).empty() ? true : false;
}


template<class T>
constexpr bool is_absolute(T const& _path) noexcept
{
	return	!is_relative(_path);
}



AMTRS_FILESYSTEM_NAMESPACE_END
#include AMTRS_DRIVER_INCLUDE(filesystem-filename.hpp)
#endif
