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
#ifndef	__libamtrs__string__convert_win32__hpp
#define	__libamtrs__string__convert_win32__hpp
#include <string>
#include <stdlib.h>
#include "../../os/win32/windows.hpp"
#include "def.hpp"
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
