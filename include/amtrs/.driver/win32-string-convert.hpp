/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__convert_win32__hpp
#define	__libamtrs__string__convert_win32__hpp
#include <string>
#include <stdlib.h>
#include "win32-api-windows.hpp"
AMTRS_NAMESPACE_BEGIN

// ==================================================================
//! 指定したロケールから wstring を生成する。
// ------------------------------------------------------------------
template<class WTraitsT = std::char_traits<wchar_t>, class WAllocatorT = std::allocator<wchar_t>, class CTraitsT = std::char_traits<char>>
inline std::basic_string<wchar_t, WTraitsT, WAllocatorT> to_wstring(std::locale _locale, std::basic_string_view<char, CTraitsT> _str)
{
	auto	prev	= std::locale::global(_locale);
	std::basic_string<wchar_t, WTraitsT, WAllocatorT>	retval;
	std::size_t	wcSize;
	mbstowcs_s(&wcSize, nullptr, 0, _str.data(), _str.size());
	retval.resize(wcSize);
	mbstowcs_s(&wcSize, retval.data(), retval.size(), _str.data(), _str.size());
	retval.resize(wcSize-1);
	std::locale::global(prev);
	return	retval;
}

// ==================================================================
//! wstring から指定したロケールで。
// ------------------------------------------------------------------
template<class TraitsT = std::char_traits<char>, class AllocatorT = std::allocator<char>, class WTraitsT = std::char_traits<char>>
inline std::basic_string<char, TraitsT, AllocatorT> to_string(std::locale _locale, std::basic_string_view<wchar_t, WTraitsT> _str)
{
	auto	prev	= std::locale::global(_locale);
	std::basic_string<char, TraitsT, AllocatorT>	retval;
	std::size_t	wcSize;
	wcstombs_s(&wcSize, nullptr, 0, _str.data(), _str.size());
	retval.resize(wcSize);
	wcstombs_s(&wcSize, retval.data(), retval.size(), _str.data(), _str.size());
	std::locale::global(prev);
	return	retval;
}

// ==================================================================
//! UTF-8 文字列から wstring を生成します。
// ------------------------------------------------------------------
template<class WTraitsT = std::char_traits<wchar_t>, class WAllocatorT = std::allocator<wchar_t>, class CTraitsT = std::char_traits<char>>
inline std::basic_string<wchar_t, WTraitsT, WAllocatorT> u8_to_wstring(std::basic_string_view<char, CTraitsT> _str)
{
	std::basic_string<wchar_t, WTraitsT, WAllocatorT>	retval;

	auto	buffSize	= ::MultiByteToWideChar(CP_UTF8, 0U, _str.data(), (int)_str.size(), nullptr, 0U);
	retval.resize(buffSize);
	::MultiByteToWideChar(CP_UTF8, 0U, _str.data(),  (int)_str.size(), retval.data(),  (int)retval.size());

	return	retval;
}

// ==================================================================
//! UTF-8 を生成します。
// ------------------------------------------------------------------
template<class StringT, class WTraitsT>
inline StringT to_u8string(std::basic_string_view<wchar_t, WTraitsT> _str)
{
	StringT	retval;

	auto	buffSize	= ::WideCharToMultiByte(CP_UTF8, 0U, _str.data(), (int)_str.size(), nullptr, 0U, nullptr, 0U);
	retval.resize(buffSize);
	::WideCharToMultiByte(CP_UTF8, 0U, _str.data(),  (int)_str.size(), retval.data(),  (int)retval.size(), nullptr, 0U);

	return	retval;
}




AMTRS_NAMESPACE_END
#endif
