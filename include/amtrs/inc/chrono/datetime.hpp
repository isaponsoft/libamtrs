/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__basic_datetime__hpp
#define	__libamtrs__basic_datetime__hpp
#include <ctime>
#include <chrono>
#include <stdexcept>
#include <locale>
#include <sstream>
#include <iomanip>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

template<class Clock>
class	basic_datetime;		// Prototype. Not uses.

// ****************************************************************************
//! 日時を簡単に扱うためのユーティリティクラス
// ----------------------------------------------------------------------------
using	datetime	= basic_datetime<std::chrono::system_clock>;


// ****************************************************************************
//! 日時を簡単に扱うためのユーティリティクラス
// ----------------------------------------------------------------------------
//! std::chrono::time_point std::tm を統合しよく使う機能を提供します。
//!	このクラスで扱われる year は 1900 年からのオフセットです。つまり、西暦2017年であれば
//! 117 を指定する必要があります。
// ----------------------------------------------------------------------------
//!	header file index.
//!		1) types.
//!		2) default constractor and operator.
//!		3) constractor and factory.
//!		4) getter
//!		5) modify
//!		6) compare
// ****************************************************************************
template<class Clock>
class	basic_datetime
{
public:
	// **************************************************************
	// types.
	// **************************************************************
	using	clock_type			= Clock;
	using	rep					= typename clock_type::rep;
	using	period				= typename clock_type::period;
	using	duration_type		= typename clock_type::duration;
	using	timepoint_type		= typename clock_type::time_point;
	using	days_type			= std::chrono::duration<rep, std::ratio<60*60*24>>;
	using	hours_type			= std::chrono::hours;
	using	minutes_type		= std::chrono::minutes;
	using	seconds_type		= std::chrono::seconds;
	using	millseconds_type	= std::chrono::milliseconds;


	// **************************************************************
	// default constractor and operator.
	// **************************************************************
	basic_datetime() = default;
	basic_datetime(const basic_datetime&) = default;
	basic_datetime& operator = (const basic_datetime&) = default;
	~basic_datetime() = default;

	// **************************************************************
	// constractor and factory.
	// **************************************************************
	basic_datetime(int _year, int _mon, int _mday, int _hours, int _min, int _sec)
	{
		m_tm.tm_year	= _year - 1900;
		m_tm.tm_mon		= _mon;
		m_tm.tm_mday	= _mday;
		m_tm.tm_hour	= _hours;
		m_tm.tm_min		= _min;
		m_tm.tm_sec		= _sec;
		m_updateDirty	= false;
		m_point			= std::chrono::system_clock::from_time_t(std::mktime(&m_tm));
	}

	basic_datetime(const std::time_t _tm)
		: m_point(std::chrono::system_clock::from_time_t(_tm))
		, m_updateDirty(true)
	{}

	basic_datetime(timepoint_type _tm)
		: m_point(_tm)
		, m_updateDirty(true)
	{}

	static basic_datetime now()
	{
		return	std::chrono::system_clock::now();
	}

	static basic_datetime from_localtime(const std::tm& _tm)
	{
		basic_datetime	retval;
		retval.m_tm				= _tm;
		retval.m_updateDirty	= false;
		retval.m_point			= std::chrono::system_clock::from_time_t(std::mktime(&retval.m_tm));
		return	retval;
	}

	static basic_datetime from_format(const char* _format, const char* _timestr)
	{
		std::tm				t{};
		std::istringstream	ss(_timestr);
		ss >> std::get_time(&t, _format);
		if (ss.fail())
		{
			throw	std::runtime_error("basic_datetime::from_format() : Time format error.");
		}
		return	from_localtime(t);
	}

	// **************************************************************
	// getter
	// **************************************************************
	timepoint_type timepoint() const noexcept
	{
		return	m_point;
	}

	operator timepoint_type () const noexcept
	{
		return	timepoint();
	}

	hours_type hours() const { return hours_type(_localtime().tm_hour); }
	minutes_type minutes() const { return minutes_type(_localtime().tm_min); }
	seconds_type seconds() const { return seconds_type(_localtime().tm_sec); }

	//! unixtime (scense 1900/1/1 0:0:0 UTC)
	std::time_t unixtime() const
	{
		return	std::chrono::system_clock::to_time_t(m_point);
	}

	//! うるう年かどうか調べます。
	static bool is_leap_year(int _year)
	{
		return	(((_year % 400) == 0) || (((_year % 4) == 0) && ((_year % 100) != 0)))
				? true
				: false;
	}

	//! 現在の年がうるう年か調べます。
	bool is_leap_year() const
	{
		return	is_leap_year(_localtime().tm_year + 1900);
	}

	//! 月の最終日を求めます。
	static int last_mday(int _year, int _mon)
	{
		constexpr int leaped[] = {31,29,31,30,31,30,31,31,30,31,30,31};
    	constexpr int none[]  = {31,28,31,30,31,30,31,31,30,31,30,31};
		return	is_leap_year(_year)
				? leaped[_mon]
				: none[_mon];
	}

	//! 現在の月の最終日を求めます。
	int last_mday(int _mon) const
	{
		auto	tm = _localtime();
		return	last_mday(tm.tm_year, _mon);
	}

	//! 現在の月の最終日を求めます。
	int last_mday() const
	{
		auto	tm = _localtime();
		return	last_mday(tm.tm_year, tm.tm_mon);
	}

	//!	format: http://en.cppreference.com/w/cpp/io/manip/put_time
	std::string format(const char* _format)
	{
		std::stringstream	ss;
		ss << std::put_time(&_localtime(), _format);
		return	ss.str();
	}

	const std::tm& localtime() const
	{
		return	_localtime();
	}


	// **************************************************************
	// modify
	// **************************************************************
	basic_datetime& add_year(int _year)
	{
		std::tm	t	= _localtime();
		t.tm_year	+= _year;
		*this	= basic_datetime::from_localtime(t);
		return	*this;
	}

	basic_datetime& add_month(int _month)
	{
		std::tm	t	= _localtime();
		t.tm_mon	+= _month;
		if (t.tm_mon > 11)
		{
			t.tm_year	+= t.tm_mon / 12;
			t.tm_mon	%= 12;
		}
		*this	= from_localtime(t);
		return	*this;
	}

	basic_datetime& add_days(int _day)
	{
		return	*this += days_type(_day);
	}

	basic_datetime& add_hours(int _hours)
	{
		return	*this += hours_type(_hours);
	}

	basic_datetime& add_minutes(int _minutes)
	{
		return	*this += minutes_type(_minutes);
	}

	basic_datetime& add_seconds(int _seconds)
	{
		return	*this += seconds_type(_seconds);
	}

	basic_datetime& set_ymd(int _year, int _month, int _mday)
	{
		std::tm	t	= _localtime();
		t.tm_year	= _year - 1900;
		t.tm_mon	= _month;
		t.tm_mday	= _mday;
		*this	= from_localtime(t);
		return	*this;
	}

	basic_datetime& set_year(int _year)
	{
		std::tm	t	= _localtime();
		t.tm_year	= _year;
		*this	= from_localtime(t);
		return	*this;
	}

	basic_datetime& set_month(int _month)
	{
		std::tm	t	= _localtime();
		t.tm_mon	= _month;
		*this	= from_localtime(t);
		return	*this;
	}

	basic_datetime& set_mday(int _mday)
	{
		std::tm	t	= _localtime();
		t.tm_mday	= _mday;
		*this	= from_localtime(t);
		return	*this;
	}
/*
	basic_datetime& set_time(int _hours, int _minutes, int _seconds)
	{
		std::tm	t = _localtime();
		t.tm_hour	= _hours;
		t.tm_min	= _minutes;
		t.tm_sec	= _seconds;
		*this	= basic_datetime::from_localtime(t);
		return	*this;
	}
*/
	basic_datetime& set_hours(int _hours)
	{
		std::tm	t = _localtime();
		t.tm_hour	= _hours;
		*this	= basic_datetime::from_localtime(t);
		return	*this;
	}

	basic_datetime& set_minutes(int _minutes)
	{
		std::tm	t = _localtime();
		t.tm_min	= _minutes;
		*this	= basic_datetime::from_localtime(t);
		return	*this;
	}

	basic_datetime& set_seconds(int _seconds)
	{
		std::tm	t = _localtime();
		t.tm_sec	= _seconds;
		*this	= basic_datetime::from_localtime(t);
		return	*this;
	}

	basic_datetime operator + (const duration_type& _offset) const
	{
		return	basic_datetime(m_point + _offset);
	}

	basic_datetime& operator += (const duration_type& _offset)
	{
		m_point 		+= _offset;
		m_updateDirty	= true;
		return	*this;
	}

	duration_type operator - (const basic_datetime& _r) const
	{
		return	m_point - _r.m_point;
	}

	basic_datetime operator - (const duration_type& _offset) const
	{
		return	basic_datetime(m_point - _offset);
	}

	basic_datetime& operator -= (const duration_type& _offset)
	{
		m_point 		-= _offset;
		m_updateDirty	= true;
		return	*this;
	}


	// **************************************************************
	// compare
	// **************************************************************
	bool operator == (const basic_datetime& _r) const { return m_point == _r.m_point; }
	bool operator != (const basic_datetime& _r) const { return m_point != _r.m_point; }
	bool operator <= (const basic_datetime& _r) const { return m_point <= _r.m_point; }
	bool operator <  (const basic_datetime& _r) const { return m_point <  _r.m_point; }
	bool operator >= (const basic_datetime& _r) const { return m_point >= _r.m_point; }
	bool operator >  (const basic_datetime& _r) const { return m_point >  _r.m_point; }

	
private:
	const std::tm& _localtime() const
	{
		if (m_updateDirty)
		{
			m_updateDirty	= false;
			std::time_t	t = std::chrono::system_clock::to_time_t(m_point);
			m_tm		= *std::localtime(&t);
		}
		return	m_tm;
	}

	timepoint_type	m_point;
	mutable std::tm	m_tm;
	mutable bool	m_updateDirty	= true;
};


template<class Clock, class Duration> bool operator == (const std::chrono::time_point<Clock, Duration>& _lhs, const basic_datetime<Clock>& _rhs) { return   _rhs == _lhs;  }
template<class Clock, class Duration> bool operator != (const std::chrono::time_point<Clock, Duration>& _lhs, const basic_datetime<Clock>& _rhs) { return !(_lhs == _rhs); }
template<class Clock, class Duration> bool operator <  (const std::chrono::time_point<Clock, Duration>& _lhs, const basic_datetime<Clock>& _rhs) { return   _rhs >= _lhs;  }
template<class Clock, class Duration> bool operator <= (const std::chrono::time_point<Clock, Duration>& _lhs, const basic_datetime<Clock>& _rhs) { return   _rhs >  _lhs;  }
template<class Clock, class Duration> bool operator >  (const std::chrono::time_point<Clock, Duration>& _lhs, const basic_datetime<Clock>& _rhs) { return   _rhs <= _lhs;  }
template<class Clock, class Duration> bool operator >= (const std::chrono::time_point<Clock, Duration>& _lhs, const basic_datetime<Clock>& _rhs) { return   _rhs <  _lhs;  }


AMTRS_NAMESPACE_END
#include AMTRS_MAKE_INCLUDE_PLATFORM(datetime)
#endif

