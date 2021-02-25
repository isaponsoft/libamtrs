/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__utf8__hpp
#define	__libamtrs__string__utf8__hpp
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
