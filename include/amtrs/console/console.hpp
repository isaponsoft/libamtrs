/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__libamtrs__console__hpp
#define	__libamtrs__libamtrs__console__hpp
#include "../amtrs.hpp"
#include <deque>
#include <functional>
#include <string>
AMTRS_CONSOLE_NAMESPACE_BEGIN


//! ログのレベル
enum class	loglevel
{
	ll_main		= 0,	//!< 主情報、ソフトウェアが目的とする情報
	ll_error	= 1,	//!< エラー
	ll_warn		= 2,	//!< エラー、警告
	ll_info		= 3,	//!< エラー、警告、必要と思われる情報
	ll_detail	= 4,	//!< エラー、警告、必要と思われる情報、詳細な情報
	ll_debug	= 5,	//!< エラー、警告、必要と思われる情報、詳細な情報、デバッグ
	ll_trace	= 6,	//!< エラー、警告、必要と思われる情報、詳細な情報、デバッグ、追跡のための情報
};


enum class progresstype
{
	none		= 0,
	extract		= 1,
	download	= 2,
	subprocess	= 3,
};


//! ログレベルを設定します
loglevel set_loglevel(loglevel _level);

//! ログを出力します
void log(loglevel _level, std::string const& _text);

//! ログを出力します(改行しません)
void logn(loglevel _level, std::string const& _text);

//! ログを出力します
void log(loglevel _level, char const* _format, ...);

//! ログを出力します(改行しません)
void logn(loglevel _level, char const* _format, ...);

void progress_start(progresstype _type, size_t _start,  size_t _end, char const* _reson);
void progress_maxsize(size_t _end);
void progress_inc(size_t _inc = 1);
void progress_inc_bandwide(int _wide, size_t _inc = 1);
void progress_inc_string(std::string_view _line, size_t _inc = 1);
void progress_end();



AMTRS_CONSOLE_NAMESPACE_END
#endif
