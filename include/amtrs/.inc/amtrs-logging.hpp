/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__basic__logging__hpp
#define	__libamtrs__basic__logging__hpp
#include <string>
#define	AMTRS_LOGGING_NAMESPACE_BEGIN	AMTRS_NAMESPACE_BEGIN namespace logging {
#define	AMTRS_LOGGING_NAMESPACE_END		} AMTRS_NAMESPACE_END
AMTRS_LOGGING_NAMESPACE_BEGIN

/* AMTRS_DEBUG_LOG_LEVEL
 *		0		Nothing
 *		1		Critical		... critical log			: 即対応しないと問題の起こる内容
 *		2		Error			... error log				: 
 *		3		Warn			... warning log				: 実行時の異常など。実行は継続できるが後々問題が発生すべき内容。
 *		4		Info			... user info log			: 実行時に注視すべき内容(データの保存を行うなど)、簡潔かつ誤解のないように書くこと
 *		5		Debug			... user debug log			: 詳細な情報(読み込んだデータの内容など)
 *		6		Trace			... user trace log			: デバッグよりかなり詳細な情報
 *		7		SysTrace		... libamtrs trace log		: libamtrs のログ。libamtrs の学習のために使用する。
 */
enum class	loglevel
{
	none		= 0,
	critical	= 1,
	error		= 2,
	warn		= 3,
	info		= 4,
	debug		= 5,
	trace		= 6,
	systemtrace	= 7
};

struct	location
{
	std::string_view	filename;
	long				line;
};

void output(loglevel _level, location _locate, std::string const& _msg, bool _return = true);
void output(std::string const& _msg, bool _return = true);


//! プラットフォームの出力命令の呼び出し
//! ユーザーから使うものではない。
void write(loglevel _level, const char* _msg, std::size_t _size, bool _return = true);


AMTRS_LOGGING_NAMESPACE_END

#if	!defined(AMTRS_DEBUG_LOG_LEVEL)
	#if		defined(DEBUG) || defined(AMTRS_DEBUG_LOG_ENABLE)
		#define	AMTRS_DEBUG_LOG_LEVEL	4
	#else
		#define	AMTRS_DEBUG_LOG_LEVEL	0
	#endif
#endif

#define	AMTRS_OUTPUT_LOG(_level, ...)	amtrs::logging::output(_level, amtrs::logging::location{__FILE__, __LINE__}, amtrs::format<std::string>(__VA_ARGS__));

#define	AMTRS_FORCE_LOG(...)	AMTRS_OUTPUT_LOG(amtrs::logging::loglevel::none, __VA_ARGS__)


// FatalLog
#if		AMTRS_DEBUG_LOG_LEVEL < 1
#define	AMTRS_CRITICAL_LOG(...)
#else
#define	AMTRS_CRITICAL_LOG(...)	AMTRS_OUTPUT_LOG(amtrs::logging::loglevel::critical, __VA_ARGS__)
#define	AMTRS_CRITICAL_LOG_USE	1
#endif

// ErrorLog
#if		AMTRS_DEBUG_LOG_LEVEL < 2
#define	AMTRS_ERROR_LOG(...)
#else
#define	AMTRS_ERROR_LOG(...)	AMTRS_OUTPUT_LOG(amtrs::logging::loglevel::error, __VA_ARGS__)
#define	AMTRS_ERROR_LOG_USE		1
#endif

// WarnLog
#if		AMTRS_DEBUG_LOG_LEVEL < 3
#define	AMTRS_WARN_LOG(...)
#else
#define	AMTRS_WARN_LOG(...)		AMTRS_OUTPUT_LOG(amtrs::logging::loglevel::warn, __VA_ARGS__)
#define	AMTRS_WARN_LOG_USE		1
#endif

// InfoLog
#if		AMTRS_DEBUG_LOG_LEVEL < 4
#define	AMTRS_INFO_LOG(...)
#else
#define	AMTRS_INFO_LOG(...)		AMTRS_OUTPUT_LOG(amtrs::logging::loglevel::info, __VA_ARGS__)
#define	AMTRS_INFO_LOG_USE		1
#endif

// DebugLog
#if		AMTRS_DEBUG_LOG_LEVEL < 5
#define	AMTRS_DEBUG_LOG(...)
#else
#define	AMTRS_DEBUG_LOG(...)	AMTRS_OUTPUT_LOG(amtrs::logging::loglevel::debug, __VA_ARGS__)
#define	AMTRS_DEBUG_LOG_USE		1
#endif

// TraceLog
#if		AMTRS_DEBUG_LOG_LEVEL < 6
#define	AMTRS_TRACE_LOG(...)
#else
#define	AMTRS_TRACE_LOG(...)	AMTRS_OUTPUT_LOG(amtrs::logging::loglevel::trace, __VA_ARGS__)
#define	AMTRS_TRACE_LOG_USE		1
#endif

// SystemTraceLog
#if		AMTRS_DEBUG_LOG_LEVEL < 7
#define	AMTRS_SYSTRACE_LOG(...)
#else
#define	AMTRS_SYSTRACE_LOG(...)	AMTRS_OUTPUT_LOG(amtrs::logging::loglevel::systemtrace, __VA_ARGS__)
#define	AMTRS_SYSTRACE_LOG_USE	1
#endif

#endif
