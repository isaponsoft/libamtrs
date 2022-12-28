/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__platforms__win32__chrono__hpp
#define	__libamtrs__platforms__win32__chrono__hpp
#include <chrono>
AMTRS_CHRONO_NAMESPACE_BEGIN


// ============================================================================
//! gmtime から localtime へ変換する。
// ----------------------------------------------------------------------------
inline std::time_t gmtime_to_localtime(std::time_t _gmtime)
{
	struct tm	t;
	gmtime_s(&t, &_gmtime);
	return	std::mktime(&t);
}


// ============================================================================
//! localtime から gmtime へ変換する。
// ----------------------------------------------------------------------------
inline std::time_t localtime_to_gmtime(std::time_t _gmtime)
{
	struct tm	t;
	localtime_s(&t, &_gmtime);
	return	std::mktime(&t);
}


// ============================================================================
//! 時間を文字列に変換する。
// ----------------------------------------------------------------------------
//!	format: http://en.cppreference.com/w/cpp/io/manip/put_time
// ----------------------------------------------------------------------------
inline std::string format(const char* _format, std::time_t _time)
{
	struct tm			t;
	std::stringstream	ss;
	localtime_s(&t, &_time);
	ss << std::put_time(&t, _format);
	return	ss.str();
}


AMTRS_CHRONO_NAMESPACE_END
#endif
