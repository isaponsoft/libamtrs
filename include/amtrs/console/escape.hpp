/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__console__escape__hpp
#define	__libamtrs__console__escape__hpp
AMTRS_CONSOLE_NAMESPACE_BEGIN

// https://en.wikipedia.org/wiki/ANSI_escape_code

static constexpr char const	clear_line[]		= { "\033[K" };
static constexpr char const	carriage_return[]	= { "\r" };
static constexpr char const	new_line[]			= { "\n" };
static constexpr char const	line_feed[]			= { "\n" };
static constexpr char const	cursor_up[]			= { "\033[A" };		// \033[nA
static constexpr char const	cursor_down[]		= { "\033[B" };		// \033[nB
static constexpr char const	cursor_right[]		= { "\033[C" };		// \033[nC
static constexpr char const	cursor_left[]		= { "\033[D" };		// \033[nD

namespace SGR {

static constexpr char const	reset[]					= { "\033[0m" };
static constexpr char const	bold[]					= { "\033[1m" };
static constexpr char const	faint[]					= { "\033[2m" };
static constexpr char const	italic[]				= { "\033[3m" };
static constexpr char const	underline[]				= { "\033[4m" };
static constexpr char const	slow_blink[]			= { "\033[5m" };
static constexpr char const	rapid_blink[]			= { "\033[6m" };
static constexpr char const	reverse[]				= { "\033[7m" };	// forground color <=> background color
static constexpr char const	hide[]					= { "\033[8m" };
static constexpr char const	strike[]				= { "\033[9m" };	// 取り消し線
static constexpr char const	font_default[]			= { "\033[10m" };
static constexpr char const	font_1[]				= { "\033[11m" };
static constexpr char const	font_2[]				= { "\033[12m" };
static constexpr char const	font_3[]				= { "\033[13m" };
static constexpr char const	font_4[]				= { "\033[14m" };
static constexpr char const	font_5[]				= { "\033[15m" };
static constexpr char const	font_6[]				= { "\033[16m" };
static constexpr char const	font_7[]				= { "\033[17m" };
static constexpr char const	font_8[]				= { "\033[18m" };
static constexpr char const	font_9[]				= { "\033[19m" };
static constexpr char const	font_gothic[]			= { "\033[20m" };
static constexpr char const	double_underline[]		= { "\033[21m" };
static constexpr char const	normal_intensity[]		= { "\033[22m" };
static constexpr char const	neither_italic[]		= { "\033[23m" };
static constexpr char const	not_underlined[]		= { "\033[24m" };
static constexpr char const	not_blinking[]			= { "\033[25m" };
static constexpr char const	proportional_spacing[]	= { "\033[26m" };
static constexpr char const	not_reversed[]			= { "\033[27m" };
static constexpr char const	reveal[]				= { "\033[28m" };
static constexpr char const	not_crossed_out[]		= { "\033[29m" };

} // SGR

static constexpr char const	fgcolor_black[]			= { "\033[30m" };
static constexpr char const	fgcolor_red[]			= { "\033[31m" };
static constexpr char const	fgcolor_green[]			= { "\033[32m" };
static constexpr char const	fgcolor_yellow[]		= { "\033[33m" };
static constexpr char const	fgcolor_blue[]			= { "\033[34m" };
static constexpr char const	fgcolor_magenta[]		= { "\033[35m" };
static constexpr char const	fgcolor_cyan[]			= { "\033[36m" };
static constexpr char const	fgcolor_white[]			= { "\033[37m" };
static constexpr char const	fgcolor_reset[]			= { "\033[39m" };
static constexpr char const	fgcolor_light_gray[]	= { "\033[90m" };
static constexpr char const	fgcolor_light_red[]		= { "\033[91m" };
static constexpr char const	fgcolor_light_green[]	= { "\033[92m" };
static constexpr char const	fgcolor_light_yellow[]	= { "\033[93m" };
static constexpr char const	fgcolor_light_blue[]	= { "\033[94m" };
static constexpr char const	fgcolor_light_magenta[]	= { "\033[95m" };
static constexpr char const	fgcolor_light_cyan[]	= { "\033[96m" };
static constexpr char const	fgcolor_light_white[]	= { "\033[97m" };

static constexpr char const	bgcolor_black[]			= { "\033[40m" };
static constexpr char const	bgcolor_red[]			= { "\033[41m" };
static constexpr char const	bgcolor_green[]			= { "\033[42m" };
static constexpr char const	bgcolor_yellow[]		= { "\033[43m" };
static constexpr char const	bgcolor_blue[]			= { "\033[44m" };
static constexpr char const	bgcolor_magenta[]		= { "\033[45m" };
static constexpr char const	bgcolor_cyan[]			= { "\033[46m" };
static constexpr char const	bgcolor_white[]			= { "\033[47m" };
static constexpr char const	bgcolor_reset[]			= { "\033[49m" };

AMTRS_CONSOLE_NAMESPACE_END
#endif
