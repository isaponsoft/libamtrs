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
#ifndef	__libamtrs__input__key_code__hpp
#define	__libamtrs__input__key_code__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

enum class key_code
{
	unkown			= 0x00,
	backspace_key	= 0x08,
	tab_key			= 0x09,
	clear_key		= 0x0c,
	return_key		= 0x0d,
	shift_key		= 0x10,
	control_key		= 0x11,
	menu_key		= 0x12,
	pause_key		= 0x13,
	capital_key		= 0x14,
	// IME
	kana_key		= 0x15,
	hangul_key		= 0x15,
	junja_key		= 0x17,
	final_key		= 0x18,		// IME "確定"
	hanja_key		= 0x19,
	kanji_key		= 0x19,
	escape_key		= 0x1b,
	convert_key		= 0x1c,
	nonconvert_key	= 0x1d,
	accept_key		= 0x1e,
	modechange_key	= 0x1f,

	space_key		= 0x20,
	prior_key		= 0x21,
	next_key		= 0x22,
	end_key			= 0x23,
	home_key		= 0x24,
	left_key		= 0x25,
	up_key			= 0x26,
	right_key		= 0x27,
	down_key		= 0x28,


	back_key		= 0x010001,	// Android Back key

	command_key		= 0x020001,	// OSX command key
};


AMTRS_NAMESPACE_END
#endif
