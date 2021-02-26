/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libschedule__save__time__hpp
#define	__libschedule__save__time__hpp
#include <ctime>
#include <amtrs/amtrs.hpp>
#include <amtrs/chrono/time.hpp>
#include "desirializer.hpp"
namespace save {

using namespace amtrs;


//! 時間型
//! std::time_t のラッパーで、主にシリアライザとヘルパ関数のために存在する。
#if	1
using	timetype	= amtrs::chrono::timev;
#else
class	timetype
{
public:
	using	int_type	= std::make_signed<std::time_t>::type;
	using	uint_type	= std::make_unsigned<std::time_t>::type;

	timetype() : v(0) {}
	timetype(timetype const&) = default;
	timetype& operator = (timetype const&) = default;

	explicit constexpr timetype(std::time_t _v) : v(_v) {}

	constexpr std::time_t get() const noexcept { return v; }

	constexpr bool operator == (timetype const& _r) const noexcept { return v == _r.v; }
	constexpr bool operator != (timetype const& _r) const noexcept { return v != _r.v; }
	constexpr bool operator <  (timetype const& _r) const noexcept { return v <  _r.v; }
	constexpr bool operator <= (timetype const& _r) const noexcept { return v <= _r.v; }
	constexpr bool operator >  (timetype const& _r) const noexcept { return v >  _r.v; }
	constexpr bool operator >= (timetype const& _r) const noexcept { return v >= _r.v; }

	static timetype now() { return timetype(std::time(nullptr)); }

	// ==============================================================
	//! 指定した時間を生成します。ローカルタイムでの指定になります。
	// --------------------------------------------------------------
	//!	_ad_year
	//!		西暦です。2020なら2020を指定します。
	//!	_mon
	//!		1 - 12 の数え月です。
	// --------------------------------------------------------------
	static timetype mktime(int _ad_year, int _mon = 1, int _mday = 1, int _hour = 0, int _min = 0, int _sec = 0)
	{
		std::tm	t;
		t.tm_year	= _ad_year - 1900;
		t.tm_mon	= _mon - 1;
		t.tm_mday	= _mday;
		t.tm_hour	= _hour;
		t.tm_min	= _min;
		t.tm_sec	= _sec;
		return timetype(std::mktime(&t));
	}

	struct tm localtime() const { return chrono::localtime(v); }

private:
	std::time_t	v;
};
#endif


//! _a が _b より前に発生するスケジュールかどうか調べます。
//! 現時刻よりも過去の時間は永遠に来ない時間として後ろに回されます。
int time_compare(timetype _now, timetype _a, timetype _b);


inline bool deserialize(timetype& _data, std::istream& _in)
{
	uint8_t	s;
	if (!read_int<uint8_t>(s, _in)) { return false; }
	switch (s)
	{
		case 0 :
		{
			uint32_t	v;
			if (!read_int<uint32_t>(v, _in)) { return false; }
			_data	= timetype(v);
			return	true;
		}
		case 1 :
		{
			uint64_t	v;
			if (!read_int<uint64_t>(v, _in)) { return false; }
			_data	= timetype(v);
			return	true;
		}
		default:
			return	false;
	}
}

inline bool serialize(std::ostream& _out, timetype const& _data)
{
	if constexpr (sizeof(std::time_t) == 4)
	{
		if (!write_int<uint8_t>(_out, 0)) { return false; }

		uint32_t	v	= static_cast<uint32_t>(_data.get());
		return	write_int<uint32_t>(_out, v);
	}
	else if constexpr (sizeof(std::time_t) == 8)
	{
		if (!write_int<uint8_t>(_out, 1)) { return false; }

		uint64_t	v	= static_cast<uint64_t>(_data.get());
		return	write_int<uint64_t>(_out, v);
	}
	else
	{
		return	false;
	}
}


}
#endif
