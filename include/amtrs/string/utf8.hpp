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
