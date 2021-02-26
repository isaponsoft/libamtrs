/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__platforms__posix__chrono__hpp
#define	__libamtrs__platforms__posix__chrono__hpp
#include <chrono>
#include <time.h>
AMTRS_CHRONO_NAMESPACE_BEGIN



// ============================================================================
//! gmtime から localtime へ変換する。
// ----------------------------------------------------------------------------
inline extern std::time_t gmtime_to_localtime(std::time_t _gmtime)
{
	return	std::mktime(gmtime(&_gmtime));
}


// ============================================================================
//! localtime から gmtime へ変換する。
// ----------------------------------------------------------------------------
inline extern std::time_t localtime_to_gmtime(std::time_t _gmtime)
{
	return	std::mktime(localtime(&_gmtime));
}


// ============================================================================
//! 時間を文字列に変換する。
// ----------------------------------------------------------------------------
//!	format: http://en.cppreference.com/w/cpp/io/manip/put_time
// ----------------------------------------------------------------------------
inline std::string format(const char* _format, std::time_t _time)
{
	std::stringstream	ss;
	ss << std::put_time(std::localtime(&_time), _format);
	return	ss.str();
}


AMTRS_CHRONO_NAMESPACE_END
#endif
