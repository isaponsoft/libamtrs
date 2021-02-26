/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <X11/keysym.h>
#include <amtrs/amtrs.hpp>
#include AMTRS_DRIVER_CUSTOM_INCLUDE(x11/input-keyboard.hpp)
AMTRS_NAMESPACE_BEGIN


key_code x11key_to_keycode(Display* _display, int _keycode)
{
#if	1
	int		ret;
	KeySym*	sym	= XGetKeyboardMapping(_display, _keycode, 1, &ret);
	if (!sym)
	{
		return	key_code::unkown;
	}
	KeySym	key	= *sym;
	XFree(sym);
#else
	KeySym	key	= XkbKeycodeToKeysym(_display, _keycode, 0, 0);
#endif

	#define	__AMTRS_KEYLAYOUT(_XK, _AK)	case _XK : return key_code::_AK;
	switch (key)
	{
	__AMTRS_KEYLAYOUT(XK_BackSpace			, backspace_key)
	__AMTRS_KEYLAYOUT(XK_Tab				, tab_key)
	__AMTRS_KEYLAYOUT(XK_Linefeed			, unkown)
	__AMTRS_KEYLAYOUT(XK_Clear				, clear_key)
	__AMTRS_KEYLAYOUT(XK_Return				, return_key)
	__AMTRS_KEYLAYOUT(XK_Pause				, pause_key)
	__AMTRS_KEYLAYOUT(XK_Scroll_Lock		, unkown)
	__AMTRS_KEYLAYOUT(XK_Sys_Req			, unkown)
	__AMTRS_KEYLAYOUT(XK_Escape				, escape_key)
	__AMTRS_KEYLAYOUT(XK_Delete				, unkown)

	// International & multi-key character composition
	__AMTRS_KEYLAYOUT(XK_Multi_key			, unkown)
#ifdef	XK_Codeinpu
	__AMTRS_KEYLAYOUT(XK_Codeinpu			, unkown)
#endif
	__AMTRS_KEYLAYOUT(XK_SingleCandidate	, unkown)
	__AMTRS_KEYLAYOUT(XK_MultipleCandidate	, unkown)
	__AMTRS_KEYLAYOUT(XK_PreviousCandidate	, unkown)

	// Japanese keyboard support
	__AMTRS_KEYLAYOUT(XK_Kanji				, unkown)
	__AMTRS_KEYLAYOUT(XK_Muhenkan			, unkown)
	__AMTRS_KEYLAYOUT(XK_Henkan_Mode		, unkown)
	//__AMTRS_KEYLAYOUT(XK_Henkan				, unkown)	// = XK_Henkan_Mode
	__AMTRS_KEYLAYOUT(XK_Romaji				, unkown)
	__AMTRS_KEYLAYOUT(XK_Hiragana			, unkown)
	__AMTRS_KEYLAYOUT(XK_Katakana			, unkown)
	__AMTRS_KEYLAYOUT(XK_Hiragana_Katakana	, unkown)
	__AMTRS_KEYLAYOUT(XK_Zenkaku			, unkown)
	__AMTRS_KEYLAYOUT(XK_Hankaku			, unkown)
	__AMTRS_KEYLAYOUT(XK_Zenkaku_Hankaku	, unkown)
	__AMTRS_KEYLAYOUT(XK_Touroku			, unkown)
	__AMTRS_KEYLAYOUT(XK_Massyo				, unkown)
	__AMTRS_KEYLAYOUT(XK_Kana_Lock			, unkown)
	__AMTRS_KEYLAYOUT(XK_Kana_Shift			, unkown)
	__AMTRS_KEYLAYOUT(XK_Eisu_Shift			, unkown)
	__AMTRS_KEYLAYOUT(XK_Eisu_toggle		, unkown)
	__AMTRS_KEYLAYOUT(XK_Kanji_Bangou		, unkown)
	//__AMTRS_KEYLAYOUT(XK_Zen_Koho			, unkown)	// = XK_MultipleCandidate
	//__AMTRS_KEYLAYOUT(XK_Mae_Koho			, unkown)	// = XK_PreviousCandidate
	//  Cursor control & motion
	__AMTRS_KEYLAYOUT(XK_Home				, home_key)
	__AMTRS_KEYLAYOUT(XK_Left				, left_key)
	__AMTRS_KEYLAYOUT(XK_Up					, up_key)
	__AMTRS_KEYLAYOUT(XK_Right				, right_key)
	__AMTRS_KEYLAYOUT(XK_Down				, down_key)
	__AMTRS_KEYLAYOUT(XK_Prior				, prior_key)	// = XK_Prior
	// __AMTRS_KEYLAYOUT(XK_Page_Up			, prior_key)	// = XK_Prior
	__AMTRS_KEYLAYOUT(XK_Next				, next_key)
	//__AMTRS_KEYLAYOUT(XK_Page_Down		, next_key)	// = XK_Next
	__AMTRS_KEYLAYOUT(XK_End				, end_key)
	__AMTRS_KEYLAYOUT(XK_Begin				, unkown)

	//  Misc functions
	__AMTRS_KEYLAYOUT(XK_Select				, unkown)
	__AMTRS_KEYLAYOUT(XK_Print				, unkown)
	__AMTRS_KEYLAYOUT(XK_Execute			, unkown)
	__AMTRS_KEYLAYOUT(XK_Insert				, unkown)
	__AMTRS_KEYLAYOUT(XK_Undo				, unkown)
	__AMTRS_KEYLAYOUT(XK_Redo				, unkown)
	__AMTRS_KEYLAYOUT(XK_Menu				, unkown)
	__AMTRS_KEYLAYOUT(XK_Find				, unkown)
	__AMTRS_KEYLAYOUT(XK_Cancel				, unkown)
	__AMTRS_KEYLAYOUT(XK_Help				, unkown)
	__AMTRS_KEYLAYOUT(XK_Break				, unkown)
	__AMTRS_KEYLAYOUT(XK_Mode_switch		, unkown)
	//__AMTRS_KEYLAYOUT(XK_script_switch	, unkown)	// = XK_Mode_switch
	__AMTRS_KEYLAYOUT(XK_Num_Lock			, unkown)

	// Keypad functions, keypad numbers cleverly chosen to map to ASCII
	__AMTRS_KEYLAYOUT(XK_KP_Space			, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Tab				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Enter			, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_F1				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_F2				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_F3				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_F4				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Home			, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Left			, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Up				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Right			, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Down			, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Prior			, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Next			, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_End				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Begin			, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Insert			, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Delete			, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Equal			, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Multiply		, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Add				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Separator		, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Subtract		, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Decimal			, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_Divide			, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_0				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_1				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_2				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_3				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_4				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_5				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_6				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_7				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_8				, unkown)
	__AMTRS_KEYLAYOUT(XK_KP_9				, unkown)

	// Auxilliary functions
	__AMTRS_KEYLAYOUT(XK_F1					, unkown)
	__AMTRS_KEYLAYOUT(XK_F2					, unkown)
	__AMTRS_KEYLAYOUT(XK_F3					, unkown)
	__AMTRS_KEYLAYOUT(XK_F4					, unkown)
	__AMTRS_KEYLAYOUT(XK_F5					, unkown)
	__AMTRS_KEYLAYOUT(XK_F6					, unkown)
	__AMTRS_KEYLAYOUT(XK_F7					, unkown)
	__AMTRS_KEYLAYOUT(XK_F8					, unkown)
	__AMTRS_KEYLAYOUT(XK_F9					, unkown)
	__AMTRS_KEYLAYOUT(XK_F10				, unkown)
	__AMTRS_KEYLAYOUT(XK_F11				, unkown)
	__AMTRS_KEYLAYOUT(XK_F12				, unkown)
	__AMTRS_KEYLAYOUT(XK_F13				, unkown)
	__AMTRS_KEYLAYOUT(XK_F14				, unkown)
	__AMTRS_KEYLAYOUT(XK_F15				, unkown)
	__AMTRS_KEYLAYOUT(XK_F16				, unkown)
	__AMTRS_KEYLAYOUT(XK_F17				, unkown)
	__AMTRS_KEYLAYOUT(XK_F18				, unkown)
	__AMTRS_KEYLAYOUT(XK_F19				, unkown)
	__AMTRS_KEYLAYOUT(XK_F20				, unkown)
	__AMTRS_KEYLAYOUT(XK_F21				, unkown)
	__AMTRS_KEYLAYOUT(XK_F22				, unkown)
	__AMTRS_KEYLAYOUT(XK_F23				, unkown)
	__AMTRS_KEYLAYOUT(XK_F24				, unkown)
	__AMTRS_KEYLAYOUT(XK_F25				, unkown)
	__AMTRS_KEYLAYOUT(XK_F26				, unkown)
	__AMTRS_KEYLAYOUT(XK_F27				, unkown)
	__AMTRS_KEYLAYOUT(XK_F28				, unkown)
	__AMTRS_KEYLAYOUT(XK_F29				, unkown)
	__AMTRS_KEYLAYOUT(XK_F30				, unkown)
	__AMTRS_KEYLAYOUT(XK_F31				, unkown)
	__AMTRS_KEYLAYOUT(XK_F32				, unkown)
	__AMTRS_KEYLAYOUT(XK_F33				, unkown)
	__AMTRS_KEYLAYOUT(XK_F34				, unkown)
	__AMTRS_KEYLAYOUT(XK_F35				, unkown)

	// Modifiers 
	__AMTRS_KEYLAYOUT(XK_Shift_L			, unkown)
	__AMTRS_KEYLAYOUT(XK_Shift_R			, unkown)
	__AMTRS_KEYLAYOUT(XK_Control_L			, unkown)
	__AMTRS_KEYLAYOUT(XK_Control_R			, unkown)
	__AMTRS_KEYLAYOUT(XK_Caps_Lock			, unkown)
	__AMTRS_KEYLAYOUT(XK_Shift_Lock			, unkown)
	__AMTRS_KEYLAYOUT(XK_Meta_L				, unkown)
	__AMTRS_KEYLAYOUT(XK_Meta_R				, unkown)
	__AMTRS_KEYLAYOUT(XK_Alt_L				, unkown)
	__AMTRS_KEYLAYOUT(XK_Alt_R				, unkown)
	__AMTRS_KEYLAYOUT(XK_Super_L			, unkown)
	__AMTRS_KEYLAYOUT(XK_Super_R			, unkown)
	__AMTRS_KEYLAYOUT(XK_Hyper_L			, unkown)
	__AMTRS_KEYLAYOUT(XK_Hyper_R			, unkown)

	// Latin 1

	default:	return	key_code::unkown;
	}
	#undef	__AMTRS_KEYLAYOUT

	return	key_code::unkown;
}

AMTRS_NAMESPACE_END
