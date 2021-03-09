/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__libamtrs__console__hpp
#define	__libamtrs__libamtrs__console__hpp
#include "filesystem.hpp"
#include <deque>
#include <functional>
#include <string>
#define	AMTRS_CONSOLE_NAMESPACE			AMTRS_NAMESPACE::console
#define	AMTRS_CONSOLE_NAMESPACE_BEGIN	namespace AMTRS_CONSOLE_NAMESPACE {
#define	AMTRS_CONSOLE_NAMESPACE_END		}
AMTRS_CONSOLE_NAMESPACE_BEGIN

static constexpr char const	clear_line[]		= { "\033[K" };
static constexpr char const	carriage_return[]	= { "\r" };
static constexpr char const	new_line[]			= { "\n" };
static constexpr char const	line_feed[]			= { "\n" };

static constexpr char const	fgcolor_black[]		= { "\033[30m" };
static constexpr char const	fgcolor_red[]		= { "\033[31m" };
static constexpr char const	fgcolor_green[]		= { "\033[32m" };
static constexpr char const	fgcolor_yellow[]	= { "\033[33m" };
static constexpr char const	fgcolor_blue[]		= { "\033[34m" };
static constexpr char const	fgcolor_magenta[]	= { "\033[35m" };
static constexpr char const	fgcolor_vyan[]		= { "\033[36m" };
static constexpr char const	fgcolor_white[]		= { "\033[37m" };
static constexpr char const	fgcolor_reset[]		= { "\033[39m" };

static constexpr char const	bgcolor_black[]		= { "\033[40m" };
static constexpr char const	bgcolor_red[]		= { "\033[41m" };
static constexpr char const	bgcolor_green[]		= { "\033[42m" };
static constexpr char const	bgcolor_yellow[]	= { "\033[43m" };
static constexpr char const	bgcolor_blue[]		= { "\033[44m" };
static constexpr char const	bgcolor_magenta[]	= { "\033[45m" };
static constexpr char const	bgcolor_vyan[]		= { "\033[46m" };
static constexpr char const	bgcolor_white[]		= { "\033[47m" };
static constexpr char const	bgcolor_reset[]		= { "\033[49m" };


enum class progresstype
{
	none		= 0,
	extract		= 1,
	download	= 2,
	subprocess	= 3,
};


void progress_start(progresstype _type, size_t _start,  size_t _end, char const* _reson);
void progress_maxsize(size_t _end);
void progress_inc(size_t _inc = 1);
void progress_inc_bandwide(int _wide, size_t _inc = 1);
void progress_inc_string(std::string_view _line, size_t _inc = 1);
void progress_end();



AMTRS_CONSOLE_NAMESPACE_END
#endif
