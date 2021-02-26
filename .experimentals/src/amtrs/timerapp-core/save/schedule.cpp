/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <fstream>
#include <sstream>
#include <iostream>
#include <amtrs/crypto/md5.hpp>
#include <amtrs/filesystem/@>
#include <amtrs/filesystem/util.hpp>
#include <amtrs/memory/endian.hpp>
#include <timerapp-core/save/fileutil.hpp>
#include <timerapp-core/save/storage.hpp>
#include <timerapp-core/save/schedule.hpp>
#include <timerapp-core/save/desirializer.hpp>
using namespace	amtrs;
namespace save {


schedule::~schedule()
{
}


//! _a が _b より前に発生するスケジュールかどうか調べます。
//! 現時刻よりも過去の時間は永遠に来ない時間として後ろに回されます。
int time_compare(timetype n, timetype a, timetype b)
{
	if (a == b)
	{
		return	0;
	}
	if (a <= n || b <= n)
	{
		if (a <= n && b <= n)
		{
			// どちらも過去の時間の場合新しいほうを先に持ってくる
			return	a < b ? 1 : -1;
		}
		// 片方だけ過去の場合は未来の時間を割きに持ってくる
		return	a <= n ? 1 : -1;
	}
	return	a <  b ? -1 : 1;
}


//! _baseから計算して次のアラーム時間を計算します。
auto schedule::timedata::compute_next_alarmtime(timetype _base) const noexcept -> timetype
{
	timetype	nexttime	= _base;

	// 曜日指定（つまりリピート）があるか無いかで処理を分ける
	if (!(flags & use_weeks))
	{
		nexttime	= begintime;


		// 時間指定があるなら時間を上書きする
		if (flags & use_repeattime)
		{
			nexttime.set(repeattime.times());
		}

		nexttime.seconds(0);
		return	nexttime;
	}
	else
	{
		if (nexttime < begintime)
		{
			nexttime	= begintime;
		}
		if (flags & use_repeattime)
		{
			nexttime.set(repeattime.times());
		}

		// 曜日が空っぽ
		if (!week)
		{
			nexttime	= begintime;
			nexttime.set(repeattime.times());
			return	nexttime;
		}

		for (int count = 0; count < 8; ++count)
		{
			auto	l	= nexttime.localtime();
			if (week & (1<<l.tm_wday))
			{
				if (nexttime > _base)
				{
					break;
				}
			}
			auto	n	= timetype(nexttime.get() + (24*60*60));
			if ((flags & use_end) && (endtime >= n))
			{
				break;
			}
			nexttime	= n;
		}
	}

	nexttime.seconds(0);
	return	nexttime;
}



// ************************************************************************????
// schedule

bool serialize(std::ostream& _out, schedule const& _data)
{
	// 基本情報
	std::ostringstream	bsc;
	if (!serialize(bsc, _data.basic)
	 || !write_chunk(_out, "BASIC", bsc.str()))
	{
		return	false;
	}

	// 時間情報
	std::ostringstream	tim;
	if (!serialize(tim, _data.time)
	 || !write_chunk(_out, "TIME", tim.str()))
	{
		return	false;
	}

	// 拡張情報
	std::ostringstream	ext;
	if (!serialize(ext, _data.extra)
	 || !write_chunk(_out, "EXTRA", ext.str()))
	{
		return	false;
	}

	// サウンド設定
	std::ostringstream	sl;
	if (!serialize(sl, _data.sounds)
	 || !write_chunk(_out, "SOUNDS", sl.str()))
	{
		return	false;
	}

	// 状態設定
	std::ostringstream	sts;
	if (!serialize(sts, _data.status)
	 || !write_chunk(_out, "STATUS", sts.str()))
	{
		return	false;
	}

	if (!write_terminate_chunk(_out))
	{
		return	false;
	}

	return	write_checksum32(_out, 0xffccffee);
}



bool deserialize(schedule& _data, std::istream& _in)
{
	auto	r	= subchunks_desirialize(_data, _in, [](auto& out, auto& in, auto identify)
	{
		if (identify == "BASIC")
		{
			return	deserialize(out.basic, in) ? 1 : 0;
		}
		if (identify == "TIME")
		{
			return	deserialize(out.time, in) ? 1 : 0;
		}
		if (identify == "EXTRA")
		{
			return	deserialize(out.extra, in) ? 1 : 0;
		}
		if (identify == "SOUNDS")
		{
			return	deserialize(out.sounds, in) ? 1 : 0;
		}
		if (identify == "STATUS")
		{
			return	deserialize(out.status, in) ? 1 : 0;
		}
		return	-1;
	});
	if (!r)
	{
		return	0;
	}
#if		defined(AMTRS_DEBUG_LOG_USE)
	{
		std::string	type;
		if (_data.time.type == schedule_type::schedule)
		{
			type	= "schedule";
		}
		else if (_data.time.type == schedule_type::alarm)
		{
			type	= "alarm";
		}
		else if (_data.time.type == schedule_type::none)
		{
			type	= "none";
		}
		else
		{
			type	= "unkown";
		}

		std::string	flg;
		if (_data.time.flags & schedule::timedata::use_begin)   { if (!flg.empty()) { flg += ":"; } flg += "beg";   }
		if (_data.time.flags & schedule::timedata::use_end)     { if (!flg.empty()) { flg += ":"; } flg += "end";   }
		if (_data.time.flags & schedule::timedata::use_repeattime) { if (!flg.empty()) { flg += ":"; } flg += "dtime"; }
		AMTRS_DEBUG_LOG("%s:%s : %s(%s), %s", _data.basic.providor.c_str(), _data.basic.identify.c_str(), type.c_str(), flg.c_str(), _data.basic.title.c_str());
	}
#endif
	return	read_checksum32(_in, 0xffccffee);
}



bool serialize(std::ostream& _out, schedule::basicdata const& _data)
{
	if (!write_int(_out, (int16_t)1)		// Version code
	 || !write_str(_out, _data.providor)
	 || !write_str(_out, _data.identify)
	 || !write_str(_out, _data.title)
	)
	{
		return	false;
	}
	return	true;
}


bool deserialize(schedule::basicdata& _data, std::istream& _in)
{
	return	versionable_deserialize(_data, _in, [](auto& out, auto& in, auto version)
	{
		if (version == 1)
		{
			// 基本情報
			if (!read_str(out.providor, in)
			 || !read_str(out.identify, in)
			 || !read_str(out.title, in)
			)
			{
				return	0;
			}
			return	1;
		}
		// 不明なバージョン
		return	-1;
	});
}



bool serialize(std::ostream& _out, schedule::timedata const& _data)
{
	if (!write_int(_out, (int16_t)1)		// Version code
	 || !write_int(_out, _data.type)
	 || !write_int(_out, _data.flags)
	 || !serialize(_out, _data.begintime)
	 || !serialize(_out, _data.endtime)
	 || !serialize(_out, _data.repeattime)
	 || !serialize(_out, _data.alarmTime)
	 || !write_int(_out, _data.week)
	)
	{
		return	false;
	}
	return	true;
}


bool deserialize(schedule::timedata& _data, std::istream& _in)
{
	return	versionable_deserialize(_data, _in, [](auto& out, auto& in, auto version)
	{
		if (version == 1)
		{
			// 基本情報
			if (!read_int(out.type, in)) { return 0; }
			if (!read_int(out.flags, in)) { return 0; }
			if (!deserialize(out.begintime, in)) { return 0; }
			if (!deserialize(out.endtime, in)) { return 0; }
			if (!deserialize(out.repeattime, in)) { return 0; }
			if (!deserialize(out.alarmTime, in)) { return 0; }
			if (!read_int(out.week, in)) { return 0; }
			return	1;
		}
		// 不明なバージョン
		return	-1;
	});
}



// ************************************************************************????
// extradata


bool serialize(std::ostream& _out, schedule::extradata const& _data)
{
	if (!write_int(_out, (int16_t)1)			// Version code
	 || !write_str(_out, "URL")
	 || !write_str(_out, _data.url)
	 || !write_str(_out, "NOTE")
	 || !write_str(_out, _data.note)
	)
	{
		return	false;
	}
	return	true;
}


bool deserialize(schedule::extradata& _data, std::istream& _in)
{
	return	versionable_deserialize(_data, _in, [](auto& out, auto& in, auto version)
	{
		if (version == 1)
		{
			return	keyvalue_desirialize(out, in, [](auto& out, auto key, auto value)
			{
				if (key == "URL")	{ out.url	= value; return 1; }
				if (key == "NOTE")	{ out.note	= value; return 1; }
				return	-1;
			}) ? 1 : 0;
		}
		else
		{
			// 不明なバージョン
			return	-1;
		}
	});
}


// ************************************************************************????
// soundlist


bool serialize(std::ostream& _out, schedule::soundlist const& _data)
{
	if (!write_int(_out, (int16_t)1)			// Version code
	 || !write_int(_out, (int16_t)_data.size())	// count
	)
	{
		return	false;
	}
	for (size_t i = 0; i < _data.size(); ++i)
	{
		if (!write_str(_out, _data[i]))
		{
			return	false;
		}
	}
	return	true;
}

bool deserialize(schedule::soundlist& _data, std::istream& _in)
{
	return	versionable_deserialize(_data, _in, [](auto& out, auto& in, auto version)
	{
		if (version == 1)
		{
			out	= {};

			int16_t	count;
			if (!read_int(count, in)) { return 0; }

			for (int i = 0; i < count; ++i)
			{
				std::string	value;
				if (!read_str(value, in)) { return 0; }
				out.push_back(std::move(value));
			}

			return	1;
		}
		else
		{
			// 不明なバージョン
			return	-1;
		}
	});
}


// ************************************************************************????
// statusdata


bool serialize(std::ostream& _out, schedule::statusdata const& _data)
{
	if (!write_int(_out, (int16_t)3)			// Version code
	 || !write_int(_out, _data.flags)			// flags
	)
	{
		return	false;
	}
	return	true;
}



bool deserialize(schedule::statusdata& _data, std::istream& _in)
{
	return	versionable_deserialize(_data, _in, [](auto& out, auto& in, auto version)
	{
		out.flags	= 0;

		struct
		{
			uint8_t			finish	= 0;			//!< 0以外の時は終了したデータ
			uint8_t			notify	= true;			//!< 通知を有効にするかどうか
			uint8_t			wakeup	= true;			//!< バックグラウンド時でもアプリを起動する
			uint8_t			loop	= true;			//!< アラーム音声を繰り返し再生する
		}	v1v2;

		if (version == 1)
		{
			if (!read_int(v1v2.finish, in)) { return 0; }
			if (v1v2.finish)	out.finish(true);
			if (v1v2.notify)	out.notify(true);
			if (v1v2.wakeup)	out.wakeup(true);
			out.loop_sound(true);
			return	1;
		}
		else if (version == 2)
		{
			for (;;)
			{
				std::string	key;
				if (!read_str(key, in))
				{
					return	-1;
				}
				if (key == "END") { break; }
				if (key == "FNS") { if (!read_int(v1v2.finish, in)) { return -1; }; continue; }
				if (key == "NTF") { if (!read_int(v1v2.notify, in)) { return -1; }; continue; }
				if (key == "WAK") { if (!read_int(v1v2.wakeup, in)) { return -1; }; continue; }
				return	-1;
			}
			if (v1v2.finish)	out.finish(true);
			if (v1v2.notify)	out.notify(true);
			if (v1v2.wakeup)	out.wakeup(true);
			out.loop_sound(true);
			return	1;
		}
		else if (version == 3)
		{
			out	= {};
			if (!read_int(out.flags, in))
			{
				return	-1;
			}
			return 1;
		}
		else
		{
			// 不明なバージョン
			return	-1;
		}
	});
}

// ************************************************************************????
// schedulelist


bool serialize(std::ostream& _out, schedulelist const& _data)
{
	if (!write_int(_out, (int16_t)1)			// Version code
	 || !write_int(_out, (int32_t)_data.size())	// count
	)
	{
		return	false;
	}
	for (auto& s : _data)
	{
		if (!serialize(_out, *s))
		{
			return	false;
		}
	}
	return	true;
}


bool deserialize(schedulelist& _data, std::istream& _in)
{
	AMTRS_INFO_LOG("Load schedulelist");
	int16_t		version;
	if (!read_int(version, _in))
	{
		return	false;
	}

	if (version == 1)
	{
		int32_t	count;

		if (!read_int(count, _in)) { return false; }

		//AMTRS_DEBUG_LOG("schedule count = %d", (int)count);
		for (int i = 0; i < count; ++i)
		{
			amtrs::ref<schedule>	s	= new schedule();
			if (!deserialize(*s, _in))
			{
				return	false;
			}
			if (s->time.type != schedule_type::none)
			{
				_data.push_back(std::move(s));
			}
		}
		return	true;
	}

	AMTRS_ERROR_LOG("setting_sound unkown version = %d", (int)version);
	return	false;
}


}
