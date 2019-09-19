/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__string__utf8__hpp
#define	__libamtrs__string__utf8__hpp
#include <cstdlib>
#include <string>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


// ==================================================================
//! UTF-8 文字列から１文字読み取ります。
// ------------------------------------------------------------------
//! バッファーオーバーランなどはチェックしません。
//! また、utf8として認識できない文字の場合は nullptr を返します。
// ------------------------------------------------------------------
inline const char* u8string_read_char(int32_t& _buff, const char* _cur)
{
	const uint8_t*	cur	= reinterpret_cast<const uint8_t*>(_cur);
	uint8_t			c	= *cur;
	if ( c         <= 0x7F)
	{
		_buff	= c;
		return	_cur + 1;
	}

	if ((c & 0xE0) == 0xC0)
	{
		_buff	= (static_cast<int32_t>(cur[0] & 0x1F) << 6)
				| (static_cast<int32_t>(cur[1] & 0x3F) << 0);
		return	_cur + 2;
	}

	if ((c & 0xF0) == 0xE0)
	{
		_buff	= (static_cast<int32_t>(cur[0] & 0x1F) <<12)
				| (static_cast<int32_t>(cur[1] & 0x3F) << 6)
				| (static_cast<int32_t>(cur[2] & 0x3F) << 0);
		return	_cur + 3;
	}

	if ((c & 0xF8) == 0xF0)
	{
		_buff	= (static_cast<int32_t>(cur[0] & 0x1F) <<18)
				| (static_cast<int32_t>(cur[1] & 0x3F) <<12)
				| (static_cast<int32_t>(cur[2] & 0x3F) << 6)
				| (static_cast<int32_t>(cur[3] & 0x3F) << 0);
		return	_cur + 4;
	}
	return	nullptr;
}


AMTRS_NAMESPACE_END
#endif
