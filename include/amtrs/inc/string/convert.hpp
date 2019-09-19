/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__string__convert__hpp
#define	__libamtrs__string__convert__hpp
#include <cstdlib>
#include <locale>
#include <string>
#include "def.hpp"
#include "utf8.hpp"
AMTRS_NAMESPACE_BEGIN

// ==================================================================
//! UTF-8 文字列から UTF32 文字列 を生成します。
// ------------------------------------------------------------------
//! この関数は不正な文字のチェックは行いません。
// ------------------------------------------------------------------
template<class WTraitsT = std::char_traits<int32_t>, class WAllocatorT = std::allocator<int32_t>, class CTraitsT = std::char_traits<char>>
inline std::basic_string<int32_t, WTraitsT, WAllocatorT> u8_to_u32string(std::basic_string_view<char, CTraitsT> _str);


template<
	class StringT    	= std::string,
	class WTraitsT  	= std::char_traits<wchar_t>
>
inline StringT to_u8string(std::basic_string_view<wchar_t, WTraitsT> _str);


template<
	class StringT    	= std::string,
	class WTraitsT  	= std::char_traits<wchar_t>,
	class WAllocatorT  	= std::allocator<wchar_t>
>
inline StringT to_u8string(const std::basic_string<wchar_t, WTraitsT, WAllocatorT>& _str)
{
	return	to_u8string<StringT>(std::basic_string_view<wchar_t, WTraitsT>(_str.data(), _str.size()));
}



AMTRS_NAMESPACE_END
#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#include "convert-win32.hpp"
#else
AMTRS_NAMESPACE_BEGIN
// ==================================================================
//! 指定したロケールから wstring を生成する。
// ------------------------------------------------------------------
template<class WTraitsT = std::char_traits<wchar_t>, class WAllocatorT = std::allocator<wchar_t>, class CTraitsT = std::char_traits<char>>
inline std::basic_string<wchar_t, WTraitsT, WAllocatorT> to_wstring(std::locale _locale, std::basic_string_view<char, CTraitsT> _str)
{
	auto	prev	= std::locale::global(_locale);
	std::basic_string<wchar_t, WTraitsT, WAllocatorT>	retval;
	auto	wcSize	= std::mbstowcs(nullptr, _str.data(), _str.size());
	retval.resize(wcSize);
	std::mbstowcs(retval.data(), _str.data(), _str.size());
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
	auto	wcSize	= std::wcstombs(nullptr, _str.data(), _str.size());
	retval.resize(wcSize);
	std::wcstombs(retval.data(), _str.data(), _str.size());
	std::locale::global(prev);
	return	retval;
}
#if	0
// ==================================================================
//! UTF-8 文字列から wstring を生成します。
// ------------------------------------------------------------------
//! ソフトウェア実装
// ------------------------------------------------------------------
template<class WTraitsT = std::char_traits<wchar_t>, class WAllocatorT = std::allocator<wchar_t>, class CTraitsT = std::char_traits<char>>
inline std::basic_string<wchar_t, WTraitsT, WAllocatorT> u8_to_wstring(std::basic_string_view<char, CTraitsT> _str)
{
	std::basic_string<wchar_t, WTraitsT, WAllocatorT>	retval;


	return	retval;
}
#endif
AMTRS_NAMESPACE_END
#endif


AMTRS_NAMESPACE_BEGIN

template<class WTraitsT, class WAllocatorT, class CTraitsT>
inline std::basic_string<int32_t, WTraitsT, WAllocatorT> u8_to_u32string(std::basic_string_view<char, CTraitsT> _str)
{
	using	char_type	= typename std::char_traits<int32_t>::char_type;

	std::size_t			size	= 0;

	// まずは必要なバッファの大きさ(＝文字数)を数える
	{
		auto*	cur	= _str.data();
		auto*	end	= cur + _str.size();
		while (cur < end)
		{
			uint8_t	c	= static_cast<uint8_t>(*cur);
			if ( c         <= 0x7F) { cur += 1; ++size; continue; }
			if ((c & 0xE0) == 0xC0) { cur += 2; ++size; continue; }
			if ((c & 0xF0) == 0xE0) { cur += 3; ++size; continue; }
			if ((c & 0xF8) == 0xF0) { cur += 4; ++size; continue; }
			++cur;
		}
	}

	std::basic_string<int32_t, WTraitsT, WAllocatorT>	retval(size);
	{
		auto*	out	= retval.data();
		auto*	cur	= static_cast<const uint8_t*>(_str.data());
		auto*	end	= cur + _str.size();
		while (size > 0)
		{
			uint8_t	c	= *cur;
			if ( c         <= 0x7F)
			{
				*out++	= c;
				cur		+= 1;
				size	-= 1;
				continue;
			}
			if ((c & 0xE0) == 0xC0)
			{
				out++	= (static_cast<char_type>(cur[0] & 0x1F) << 6)
						| (static_cast<char_type>(cur[1] & 0x3F) << 0);
				cur		+= 2;
				size	-= 1;
				continue;
			}
			if ((c & 0xF0) == 0xE0)
			{
				out++	= (static_cast<char_type>(cur[0] & 0x1F) <<12)
						| (static_cast<char_type>(cur[1] & 0x3F) << 6)
						| (static_cast<char_type>(cur[2] & 0x3F) << 0);
				cur		+= 3;
				size	-= 1;
				continue;
			}
			if ((c & 0xF8) == 0xF0)
			{
				out++	= (static_cast<char_type>(cur[0] & 0x1F) <<18)
						| (static_cast<char_type>(cur[1] & 0x3F) <<12)
						| (static_cast<char_type>(cur[2] & 0x3F) << 6)
						| (static_cast<char_type>(cur[3] & 0x3F) << 0);
				cur		+= 4;
				size	-= 1;
				continue;
			}
			--size;
			++cur;
		}
	}
	return	retval;
}
AMTRS_NAMESPACE_END

#endif
