/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__functions__hpp
#define	__libamtrs__functions__hpp
AMTRS_CHRONO_NAMESPACE_BEGIN


using	time_t	= std::time_t;		// 64bit以上の time_tを使用する。

/*
inline struct tm gmtime(time_t _t)
{
	return	*std::gmtime(&_t);
}

inline struct tm localtime(time_t _t)
{
	static thread_local struct tm	r;
	static thread_local time_t		t	= 0;
	if (t != _t)
	{
		t	= _t;
		r	= *std::localtime(&t);
	}
	return	r;
}
*/

// ============================================================================
//! 閏年かどうか調べる。
// ----------------------------------------------------------------------------
constexpr bool isleap(int _year) noexcept
{
	return	(((_year % 400) == 0) || (((_year % 4) == 0) && ((_year % 100) != 0)))
			? true
			: false;
}




// ============================================================================
//! 各月のmdayを返す
// ----------------------------------------------------------------------------
constexpr int last_mday(int _year, int _mon) noexcept
{
	constexpr int mday[2][12] =
	{
		{31,29,31,30,31,30,31,31,30,31,30,31},		// leap
		{31,28,31,30,31,30,31,31,30,31,30,31}		// no leap
	};
	return	mday[isleap(_year) ? 0 : 1][_mon];
}



// ============================================================================
//! 時間を文字列に変換する。
// ----------------------------------------------------------------------------
//!	format: http://en.cppreference.com/w/cpp/io/manip/put_time
// ----------------------------------------------------------------------------
template<class Clock, class Duration>
inline std::string format(const char* _format, std::chrono::time_point<Clock, Duration> _time)
{
	return	format(_format, std::chrono::system_clock::to_time_t(_time));
}



AMTRS_CHRONO_NAMESPACE_END
#endif

