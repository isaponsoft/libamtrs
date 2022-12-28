/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__chrono__time__hpp
#define	__libamtrs__chrono__time__hpp
AMTRS_CHRONO_NAMESPACE_BEGIN

// ***************************************************************************
//
// std::time_t(win32bitでは __time64_t)に対するユーティリティです。
//
// example)
//	std::time_t	t	= now() | year_t{2020} | days{12-1,10,2}
//
//
// ***************************************************************************

enum	year_t		: int {};
enum	month_t		: int {};
enum	mday_t		: int {};
enum	hour_t		: int {};
enum	minutes_t	: int {};
enum	seconds_t	: int {};
enum	wday_t		: int {};		//!< 曜日を示す。
enum	weeks_t		: int {};		//!< 何週かを示す。週単位で時間を進める場合に使用する。
enum	lastmday_t  : int {};		//!< 月の最終日を示す

/*
 * 年、月、日を表す構造体
 */
struct	days
{
	int	year	= 0;				//!< 西暦
	int	mon		= 0;
	int	day		= 0;

	days() = default;
	days(days const&) = default;
	days& operator = (days const&) = default;

	days(time_t);
	days(struct tm&);
	days(int _y, int _m, int _d) : year(_y), mon(_m), day(_d) {}

	bool operator == (days const& _b) const noexcept;
	bool operator != (days const& _b) const noexcept;
	bool operator <  (days const& _b) const noexcept;
	bool operator <= (days const& _b) const noexcept;
	bool operator >  (days const& _b) const noexcept;
	bool operator >= (days const& _b) const noexcept;
};

/*
 * 時、分、秒を表す構造体
 */
struct	times
{
	int	hour	= 0;
	int	min		= 0;
	int	sec		= 0;

	times() = default;
	times(times const&) = default;
	times& operator = (times const&) = default;

	times(time_t);
	times(struct tm&);
	times(int _h, int _m, int _s) : hour(_h), min(_m), sec(_s) {}

	bool operator == (times const& _b) const noexcept;
	bool operator != (times const& _b) const noexcept;
	bool operator <  (times const& _b) const noexcept;
	bool operator <= (times const& _b) const noexcept;
	bool operator >  (times const& _b) const noexcept;
	bool operator >= (times const& _b) const noexcept;
};


//! 時間型
//! std::time_t のラッパーです。便利な機能を追加します。
class	timev
{
public:
	using	int_type	= std::make_signed<std::time_t>::type;
	using	uint_type	= std::make_unsigned<std::time_t>::type;

	timev() : v(0) {}
	timev(timev const&) = default;
	timev& operator = (timev const&) = default;

	timev(days const& d)
	{
		*this	= mktime(d.year, d.mon, d.day, 0, 0, 0);
	}

	timev(days const& d, times const& t)
	{
		*this	= mktime(d.year, d.mon, d.day, t.hour, t.min, t.sec);
	}

	explicit constexpr timev(std::time_t _v) : v(_v) {}

	constexpr std::time_t get() const noexcept { return v; }

	constexpr bool operator == (timev const& _r) const noexcept { return v == _r.v; }
	constexpr bool operator != (timev const& _r) const noexcept { return v != _r.v; }
	constexpr bool operator <  (timev const& _r) const noexcept { return v <  _r.v; }
	constexpr bool operator <= (timev const& _r) const noexcept { return v <= _r.v; }
	constexpr bool operator >  (timev const& _r) const noexcept { return v >  _r.v; }
	constexpr bool operator >= (timev const& _r) const noexcept { return v >= _r.v; }

	static timev now() { return timev(std::time(nullptr)); }

	// ==============================================================
	//! 指定した時間を生成します。ローカルタイムでの指定になります。
	// --------------------------------------------------------------
	//!	_ad_year
	//!		西暦です。2020なら2020を指定します。
	//!	_mon
	//!		1 - 12 の数え月です。
	// --------------------------------------------------------------
	static timev mktime(int _ad_year, int _mon = 1, int _mday = 1, int _hour = 0, int _min = 0, int _sec = 0)
	{
		std::tm	t{};
		t.tm_year	= _ad_year - 1900;
		t.tm_mon	= _mon - 1;
		t.tm_mday	= _mday;
		t.tm_hour	= _hour;
		t.tm_min	= _min;
		t.tm_sec	= _sec;
		return timev(std::mktime(&t));
	}

	struct tm localtime() const
	{
		static thread_local	std::time_t	t1	= 0;
		static thread_local	struct tm	t2;
		if (t1 != v)
		{
			t1	= v;
			t2	= *std::localtime(&t1);
		}
		return t2;
	}

	inline int year   () const noexcept { return localtime().tm_year + 1900; }
	inline int month  () const noexcept { return localtime().tm_mon + 1; }
	inline int mday   () const noexcept { return localtime().tm_mday; }
	inline int wday   () const noexcept { return localtime().tm_wday; }
	inline int hour   () const noexcept { return localtime().tm_hour; }
	inline int minutes() const noexcept { return localtime().tm_min; }
	inline int seconds() const noexcept { return localtime().tm_sec; }


	inline timev& year(int _val) { auto t = localtime(); t.tm_year = _val - 1900; v = std::mktime(&t); return *this; }
	inline timev& month(int _val) { auto t = localtime(); t.tm_mon = _val - 1; v = std::mktime(&t); return *this; }
	inline timev& mday(int _val) { auto t = localtime(); t.tm_mday = _val; v = std::mktime(&t); return *this; }
	inline timev& hour(int _val) { auto t = localtime(); t.tm_hour = _val; v = std::mktime(&t); return *this; }
	inline timev& minutes(int _val) { auto t = localtime(); t.tm_min = _val; v = std::mktime(&t); return *this; }
	inline timev& seconds(int _val) { auto t = localtime(); t.tm_sec = _val; v = std::mktime(&t); return *this; }

	timev& operator = (struct days _days)
	{
		auto	t	= localtime();
		t.tm_year	= _days.year - 1900;
		t.tm_mon	= _days.mon - 1;
		t.tm_mday	= _days.day;
		v			= std::mktime(&t);
		return	*this;
	}

	timev& operator = (struct times _time)
	{
		auto	t	= localtime();
		t.tm_hour	= _time.hour;
		t.tm_min	= _time.min;
		t.tm_sec	= _time.sec;
		v			= std::mktime(&t);
		return	*this;
	}

	timev& set(struct days _days)
	{
		auto	t	= localtime();
		t.tm_year	= _days.year - 1900;
		t.tm_mon	= _days.mon - 1;
		t.tm_mday	= _days.day;
		v			= std::mktime(&t);
		return	*this;
	}

	timev& set(struct times _time)
	{
		auto	t	= localtime();
		t.tm_hour	= _time.hour;
		t.tm_min	= _time.min;
		t.tm_sec	= _time.sec;
		v			= std::mktime(&t);
		return	*this;
	}

	auto day() const -> struct days
	{
		auto	t	= localtime();
		return	{t.tm_year + 1900, t.tm_mon + 1, t.tm_mday};
	}

	auto time() const -> struct times
	{
		auto	t	= localtime();
		return	{t.tm_hour, t.tm_min, t.tm_sec};
	}

	std::string format(const char* _format) const
	{
		std::stringstream	ss;
		auto				lt	= localtime();
		ss << std::put_time(&lt, _format);
		return	ss.str();
	}

private:
	std::time_t	v;
};


inline time_t now()
{
	time_t	r;
	return	time(&r);
}



// 各要素を取り出します。
inline int year(struct tm& _t) { return _t.tm_year + 1900; }
inline int month(struct tm& _t) { return _t.tm_mon + 1; }
inline int mday(struct tm& _t) { return _t.tm_mday; }
inline int wday(struct tm& _t) { return _t.tm_wday; }
inline int hour(struct tm& _t) { return _t.tm_hour; }
inline int minutes(struct tm& _t) { return _t.tm_min; }
inline int seconds(struct tm& _t) { return _t.tm_sec; }

inline int year(time_t _t) { return localtime(_t).tm_year + 1900; }
inline int month(time_t _t) { return localtime(_t).tm_mon + 1; }
inline int mday(time_t _t) { return localtime(_t).tm_mday; }
inline int wday(time_t _t) { return localtime(_t).tm_wday; }
inline int hour(time_t _t) { return localtime(_t).tm_hour; }
inline int minutes(time_t _t) { return localtime(_t).tm_min; }
inline int seconds(time_t _t) { return localtime(_t).tm_sec; }


inline int year   (timev const& _t) { return year   (_t.get()); }
inline int month  (timev const& _t) { return month  (_t.get()); }
inline int mday   (timev const& _t) { return mday   (_t.get()); }
inline int wday   (timev const& _t) { return wday   (_t.get()); }
inline int hour   (timev const& _t) { return hour   (_t.get()); }
inline int minutes(timev const& _t) { return minutes(_t.get()); }
inline int seconds(timev const& _t) { return seconds(_t.get()); }


inline time_t operator + (time_t _a, days _b)
{
	struct tm	t	= localtime(_a);
	t.tm_year	+= _b.year;
	t.tm_mon	+= _b.mon;
	t.tm_mday	+= _b.day;
	return	mktime(&t);
}


inline time_t operator | (time_t _a, days _b)
{
	struct tm	t	= localtime(_a);
	t.tm_year	= _b.year - 1900;
	t.tm_mon	= _b.mon - 1;
	t.tm_mday	= _b.day;
	return	mktime(&t);
}


inline time_t operator + (time_t _a, times _b)
{
	return	_a + (_b.hour * 60 * 60) + (_b.min * 60) + _b.sec;
}

inline time_t operator | (time_t _a, times _b)
{
	auto	t	= localtime(_a);
	t.tm_hour	= _b.hour;
	t.tm_min	= _b.min;
	t.tm_sec	= _b.sec;
	return	mktime(&t);
}


inline time_t operator + (time_t _a, year_t _b)
{
	auto	t	= localtime(_a);
	t.tm_year	+= _b;
	return	mktime(&t);
}


inline time_t operator | (time_t _a, year_t _b)
{
	auto	t	= localtime(_a);
	t.tm_year	= _b - 1900;
	return	mktime(&t);
}


inline time_t operator + (time_t _a, month_t _b)
{
	auto	t	= localtime(_a);
	t.tm_mon	+= _b;
	return	mktime(&t);
}


inline time_t operator | (time_t _a, month_t _b)
{
	auto	t	= localtime(_a);
	t.tm_mon	= _b - 1;
	return	mktime(&t);
}


inline time_t operator + (time_t _a, mday_t _b)
{
	auto	t	= localtime(_a);
	t.tm_mday	+= _b;
	return	mktime(&t);
}


inline time_t operator | (time_t _a, mday_t _b)
{
	auto	t	= localtime(_a);
	t.tm_mday	= _b;
	return	mktime(&t);
}


inline time_t operator + (time_t _a, hour_t _b)
{
	auto	t	= localtime(_a);
	t.tm_hour	+= _b;
	return	mktime(&t);
}


inline time_t operator | (time_t _a, hour_t _b)
{
	auto	t	= localtime(_a);
	t.tm_hour	= _b;
	return	mktime(&t);
}


inline time_t operator + (time_t _a, minutes_t _b)
{
	auto	t	= localtime(_a);
	t.tm_min	+= _b;
	return	mktime(&t);
}


inline time_t operator | (time_t _a, minutes_t _b)
{
	auto	t	= localtime(_a);
	t.tm_min	= _b;
	return	mktime(&t);
}


inline time_t operator + (time_t _a, seconds_t _b)
{
	auto	t	= localtime(_a);
	t.tm_sec	+= _b;
	return	mktime(&t);
}


inline time_t operator | (time_t _a, seconds_t _b)
{
	auto	t	= localtime(_a);
	t.tm_sec	= _b;
	return	mktime(&t);
}


//! 週単位で日付を進めます。
inline time_t operator + (time_t _a, weeks_t _b)
{
	auto	t	= localtime(_a);
	t.tm_mday	+= _b * 7;
	return	mktime(&t);
}


//! 月の最終日を設定します。
inline time_t operator | (time_t _a, lastmday_t)
{
	auto	t	= localtime(_a);
	t.tm_mday	= last_mday(t.tm_year + 1900, t.tm_mon);
	return	mktime(&t);
}


//! 曜日を変更します。
inline time_t operator | (time_t _a, wday_t _b)
{
	auto	t	= localtime(_a);
	t.tm_mday	+= _b - t.tm_wday;
	return	mktime(&t);
}



inline time_t operator + (days const& _a, times const& _b)
{
	struct tm	w;
	w.tm_year	= _a.year - 1900;
	w.tm_mon	= _a.mon - 1;
	w.tm_mday	= _a.day;
	w.tm_hour	= _b.hour;
	w.tm_min	= _b.min;
	w.tm_sec	= _b.sec;
	return	mktime(&w);
}


inline time_t operator | (days const& _a, times const& _b)
{
	return	_a + _b;
}



inline days::days(time_t _t)
	: year(AMTRS_CHRONO_NAMESPACE::year(_t))
	, mon(AMTRS_CHRONO_NAMESPACE::month(_t))
	, day(AMTRS_CHRONO_NAMESPACE::mday(_t))
{}

inline days::days(struct tm& _t)
	: year(_t.tm_year + 1900)
	, mon(_t.tm_mon + 1)
	, day(_t.tm_mday)
{}



inline bool days::operator == (days const& _b) const noexcept
{
	return	year == _b.year && mon == _b.mon && day == _b.day;
}

inline bool days::operator != (days const& _b) const noexcept
{
	return	!(*this == _b);
}

inline bool days::operator <  (days const& _b) const noexcept
{
	if (year < _b.year)	return	true;
	if (year > _b.year)	return	false;
	if (mon  < _b.mon )	return	true;
	if (mon  > _b.mon )	return	false;
	if (day  < _b.day )	return	true;
	if (day  > _b.day )	return	false;
	return	false;
}

inline bool days::operator <= (days const& _b) const noexcept
{
	if (year < _b.year)	return	true;
	if (year > _b.year)	return	false;
	if (mon  < _b.mon )	return	true;
	if (mon  > _b.mon )	return	false;
	if (day  < _b.day )	return	true;
	if (day  > _b.day )	return	false;
	return	true;
}

inline bool days::operator >  (days const& _b) const noexcept
{
	return	_b <= *this;
}

inline bool days::operator >= (days const& _b) const noexcept
{
	return	_b < *this;
}



inline times::times(time_t _t)
	: hour(AMTRS_CHRONO_NAMESPACE::hour(_t))
	, min(AMTRS_CHRONO_NAMESPACE::minutes(_t))
	, sec(AMTRS_CHRONO_NAMESPACE::seconds(_t))
{}

inline times::times(struct tm& _t)
	: hour(_t.tm_hour)
	, min(_t.tm_min)
	, sec(_t.tm_sec)
{}


inline bool times::operator == (times const& _b) const noexcept
{
	return	hour == _b.hour && min == _b.min && sec == _b.sec;
}

inline bool times::operator != (times const& _b) const noexcept
{
	return	!(*this == _b);
}

inline bool times::operator <  (times const& _b) const noexcept
{
	if (hour < _b.hour)	return	true;
	if (hour > _b.hour)	return	false;
	if (min  < _b.min )	return	true;
	if (min  > _b.min )	return	false;
	if (sec  < _b.sec )	return	true;
	if (sec  > _b.sec )	return	false;
	return	false;
}

inline bool times::operator <= (times const& _b) const noexcept
{
	if (hour < _b.hour)	return	true;
	if (hour > _b.hour)	return	false;
	if (min  < _b.min )	return	true;
	if (min  > _b.min )	return	false;
	if (sec  < _b.sec )	return	true;
	if (sec  > _b.sec )	return	false;
	return	true;
}

inline bool times::operator >  (times const& _b) const noexcept
{
	return	_b <= *this;
}

inline bool times::operator >= (times const& _b) const noexcept
{
	return	_b < *this;
}


AMTRS_CHRONO_NAMESPACE_END
AMTRS_NAMESPACE_BEGIN

template<class Out>
bool serialize(Out& _out, chrono::timev const& _val)
{
	if (!_out)
	{
		return	false;
	}
	// サイズ情報
	if (!serialize(_out, static_cast<uint8_t>(1)))
	{
		return	false;
	}
	uint64_t	v	= _val.get();
	return	serialize(_out, v);
}


template<class In>
bool deserialize(chrono::timev& _buff, In& _in)
{
	if (!_in)
	{
		return	false;
	}
	uint8_t	size;
	if (!deserialize(size, _in))
	{
		return	false;
	}
	if (size == 0)
	{
		uint32_t	v;
		if (!deserialize(v, _in)) { return false; }
		_buff	= chrono::timev((std::time_t)v);
	}
	else if (size == 1)
	{
		uint64_t	v;
		if (!deserialize(v, _in)) { return false; }
		_buff	= chrono::timev((std::time_t)v);
	}
	else
	{
		return	false;
	}
	return	_in;
}

AMTRS_NAMESPACE_END
#endif
