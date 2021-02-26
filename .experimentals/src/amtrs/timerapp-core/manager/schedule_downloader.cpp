/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <chrono>
#include <sstream>
#include <thread>
#include <amtrs/application/@>
#include <amtrs/chrono/@>
#include <amtrs/chrono/calendar.hpp>
#include <amtrs/net/http/@>
#include <amtrs/logging/@>
#include <amtrs/filesystem/@>
#include <amtrs/string/@>
#include <timerapp-core/manager/schedule_downloader.hpp>
#include <timerapp-core/save/schedule.hpp>
#include <timerapp-core/save/desirializer.hpp>
#include <timerapp-core/save/fileutil.hpp>
#include <timerapp-core/save/time.hpp>

using namespace amtrs;
using namespace save;

namespace	fs				= amtrs::filesystem;

using		schedule_ref	= amtrs::ref<save::schedule>;

struct	calendardata
{
	save::timetype							synctime;	//!< 最後に同期した時間
	schedule_source							source;		//!< 入力もとに関する情報
	std::vector<chrono::calendar::event>	events;		//!< カレンダーの持っているイベント
};
static	std::list<calendardata>					gCalendars;


static	bool									gUpdate			= false;
static	bool									gSyncronizing	= false;
static	schedule_downloader::status_t			sStatus			= schedule_downloader::status_t::busy;



static	bool load_items();


void schedule_downloader::update(std::string _calendar_id, amtrs::chrono::calendar::event _event)
{
}


void schedule_downloader::remove(std::string _calendar_id, std::string _event_id)
{
	for (auto& cal : gCalendars)
	{
		if (cal.source.identify == _calendar_id)
		{
			auto	it	= std::remove_if(cal.events.begin(), cal.events.end(), [&](auto& a)
						{
							return	a.identify == _event_id;
						});
			if (it != cal.events.end())
			{
				AMTRS_DEBUG_LOG("Remove %s:%s, title=%s", _calendar_id.c_str(), _event_id.c_str(), it->title.c_str());
				cal.events.erase(it, cal.events.end());
				gUpdate	= true;
				return;
			}
		}
	}
	AMTRS_DEBUG_LOG("Not remove %s:%s", _calendar_id.c_str(), _event_id.c_str());
}



// ========================================================================
//! 同期を開始する。
// ------------------------------------------------------------------------
void schedule_downloader::initialize()
{
	load_items();
}



// ========================================================================
//! 通信を開始する。
// ------------------------------------------------------------------------
void schedule_downloader::start()
{
	gSyncronizing	= true;
	chrono::calendar::permission([&](auto ok)
	{
		for (auto& e : gCalendars)
		{
			AMTRS_DEBUG_LOG("%s, %s, %s", e.source.identify.c_str(), e.source.title.c_str(), e.source.url.c_str());
		}


		// 読み込み対象のカレンダー
		std::list<ref<chrono::calendar>>	calendars;

		// 非システムのカレンダーを列挙
		for (auto& cal : gCalendars)
		{
			if (!cal.source.url.empty())
			{
				calendars.push_back(amtrs::chrono::calendar::openev_with_url(cal.source.url, cal.source.title));
			}
		}
		if (ok)
		{
			// システムカレンダーを列挙
			chrono::calendar::enumrate(0, [&](auto _calendar)
			{
				AMTRS_TRACE_LOG("Calendar identifty=%s, name=%s, account=%s", _calendar->identify().c_str(), _calendar->name().c_str(), _calendar->account().c_str());
				calendars.push_back(_calendar);
				auto	m	= std::find_if(gCalendars.begin(), gCalendars.end(), [&](auto& c)
							{
								return	_calendar->identify() == c.source.identify;
							});
				// 存在しないカレンダーなので追加する
				if (m == gCalendars.end())
				{
					gCalendars.push_back({
						.synctime	= save::timetype(-1),
						.source		= {
							.calendar	= _calendar,
							.identify	= _calendar->identify(),
							.title		= _calendar->name(),
							.url		= ""
						},
						.events		= {}
					});
				}
			});
		}


		// 対象にないカレンダーを削除
		for (auto it = gCalendars.begin(); it != gCalendars.end(); )
		{
			auto	m	= std::find_if(calendars.begin(), calendars.end(), [=](auto& c)
						{
							return	c->identify() == it->source.identify;
						});
			if (m == calendars.end())
			{
				it	= gCalendars.erase(it);
			}
			else
			{
				++it;
			}
		}


		// バックグラウンドで読み込み
		std::thread([=]()
		{
			for (auto& cal : calendars)
			{
				AMTRS_TRACE_LOG("Calendar identifty=%s, name=%s, account=%s", cal->identify().c_str(), cal->name().c_str(), cal->account().c_str());
				std::vector<chrono::calendar::event>	events;
				cal->events([&](auto& e)
				{
					events.push_back(e);
				});
				auto	it	= std::find_if(gCalendars.begin(), gCalendars.end(), [=](auto& c)
				{
					return	c.source.identify == cal->identify();
				});
				it->events	= std::move(events);
			}
			save();
			gSyncronizing			= false;
			sStatus					= schedule_downloader::success;
		}).detach();
	});

}


// ========================================================================
//! システムカレンダーの同期を行う
// ------------------------------------------------------------------------
void schedule_downloader::sync(std::string const& _calendarId)
{
	AMTRS_INFO_LOG("-------------------------------------- sync:%s", _calendarId.c_str());
	chrono::calendar::enumrate(0, [&](auto _calendar)
	{
		if (_calendar->identify() != _calendarId)
		{
			return;
		}
		auto	m	= std::find_if(gCalendars.begin(), gCalendars.end(), [&](auto& c)
					{
						return	_calendar->identify() == c.source.identify;
					});
		if (m == gCalendars.end())
		{
			AMTRS_INFO_LOG("not online %s", _calendarId.c_str());
			return;
		}

		std::vector<chrono::calendar::event>	events;
		_calendar->events([&](auto& e)
		{
			events.push_back(e);
		});
		m->events	= std::move(events);
	});
	gUpdate	= true;
	sStatus	= schedule_downloader::success;
}



// ========================================================================
//! 同期を停止する。	
// ------------------------------------------------------------------------
void schedule_downloader::pause()
{
	AMTRS_DEBUG_LOG("ScheduleDownloader::pause");
}


// ========================================================================
//! 現在のステータスを取得する。
// ------------------------------------------------------------------------
auto schedule_downloader::state() -> status_t
{
	return	sStatus;
}



// ========================================================================
//! スケジュールを指定するバッファに反映させます。
// ------------------------------------------------------------------------
bool schedule_downloader::update_to(save::schedulelist& _buffer)
{
	AMTRS_DEBUG_LOG("Schedule downloaded data update to");
	auto	now		= chrono::timev::now();
	bool	retflg	= gUpdate;


	// 有効なデータの一覧を生成する
	std::vector<std::pair<calendardata const*, chrono::calendar::event const*>>	enables;
	for (auto& cal : gCalendars)
	{
		// 無効化されている
		if (!cal.source.enable()) { continue; }

		for (auto& e : cal.events)
		{
			if (cal.source.older()								// 古い時間も表示
			||  (e.start.get() > now.get())						// 未来のイベント
			||  (now.get() - e.start.get()) < (30*24*60*60))	// 30日以上経過していない
			{
				enables.push_back({ &cal, &e });
			}
		}
	}


	// なくなったデータの削除と存在するデータの更新
	for (auto itm = _buffer.begin(); itm != _buffer.end(); )
	{
		if ((*itm)->basic.providor.empty())
		{
			++itm;
			continue;
		}

		auto	m	= std::find_if(enables.begin(), enables.end(), [=](auto& i)
					{
						return	i.first->source.identify == (*itm)->basic.providor
							&&	i.second->identify == (*itm)->basic.identify;
					});
		if (m == enables.end())
		{
			itm	= _buffer.erase(itm);
		}
		else
		{
			(*itm)->basic.title		= (m)->second->title;
			(*itm)->time.begintime	= (m)->second->start;
			(*itm)->time.endtime	= (m)->second->end;
			(*itm)->time.refresh_alart(now);
			(*itm)->extra.note		= (m)->second->description;
//			(*itm)->noedit	= true;
			++itm;
		}
	}

	// 新しいデータの追加
	for (auto& i : enables)
	{
		auto	m	= std::find_if(_buffer.begin(), _buffer.end(), [&](auto itm)
					{
						return	itm->basic.providor == i.first->source.identify && itm->basic.identify == i.second->identify;
					});
		if (m != _buffer.end())
		{
			continue;
		}
		ref<save::schedule>	s	= new save::schedule();
		s->basic.providor	= i.first->source.identify;
		s->basic.identify	= i.second->identify;
		s->basic.title		= i.second->title;
		s->time.type		= save::schedule_type::schedule;
		s->time.flags		= save::schedule::timedata::use_begin;
		s->time.begintime	= i.second->start;
		s->time.endtime		= i.second->end;
		s->extra.note		= i.second->description;
//		s->noedit	= true;
		s->time.refresh_alart(now);
		_buffer.push_back(s);
	}

	gUpdate	= false;
	return	retflg;
}




void schedule_downloader::add_eventsource(schedule_source const& _es)
{
	for (auto& e : gCalendars)
	{
		if (e.source.identify == _es.identify || e.source.title == _es.title || e.source.url == _es.url)
		{
			AMTRS_DEBUG_LOG("override event source %s, %s", _es.title.c_str(), _es.url.c_str());
			e.source.identify	= _es.identify;
			e.source.title		= _es.title;
			e.source.url		= _es.url;
			return;
		}
	}
	AMTRS_DEBUG_LOG("add event source %s : %s, %s", _es.identify.c_str(), _es.title.c_str(), _es.url.c_str());
	gCalendars.push_back({
		.synctime	= save::timetype(-1),
		.source		= _es,
		.events		= {}
	});
}



std::string filename()
{
	return	amtrs::application::get_application()->files_dir() + "/netschedule.cache";
}




void schedule_downloader::enumrate_source(std::function<void(schedule_source*)> _callback)
{
	for (auto&e : gCalendars)
	{
		_callback(&e.source);
	}
}



bool serialize(std::ostream& _out, schedule_source const& _data)
{
	// バージョンと同期時刻を保存しておく
	if (!serialize(_out, (int16_t)1)
	 || !serialize(_out, _data.identify)
	 || !serialize(_out, _data.title)
	 || !serialize(_out, _data.url)
	 || !serialize(_out, _data.flags)
	)
	{
		return	false;
	}
	return	true;
}





bool serialize(std::ostream& _out, calendardata const& _data)
{
	// バージョンと同期時刻を保存しておく
	if (!serialize(_out, (int16_t)2)
	 || !serialize(_out, _data.synctime)
	)
	{
		return	false;
	}

	// ソース情報を保存
	std::ostringstream	src;
	if (!serialize(src, _data.source)
	 || !write_chunk(_out, "SOURCE", src.str()))
	{
		return	false;
	}

	// イベントキャッシュ
	std::ostringstream	evnts;
	if (!serialize(evnts, _data.events)
	 || !write_chunk(_out, "EVENTS", evnts.str()))
	{
		return	false;
	}


	if (!write_terminate_chunk(_out))
	{
		return	false;
	}

	return	write_checksum32(_out, 0xffccffee);
}


bool deserialize(schedule_source& _out, std::istream& _in)
{
	int16_t	version;
	if (!deserialize(version, _in))
	{
		return	0;
	}
	if (version == 1)
	{
		if (!deserialize(_out.identify, _in)
		 || !deserialize(_out.title, _in)
		 || !deserialize(_out.url, _in)
		 || !deserialize(_out.flags, _in)
		)
		{
			return	0;
		}
	}
	return	-1;
}


bool deserialize(calendardata& _out, std::istream& _in)
{
	int16_t	version;
	if (!deserialize(version, _in))
	{
		return	false;
	}
	if (version == 2)
	{
		if (!deserialize(_out.synctime, _in))
		{
			return	false;
		}

		auto	r	= subchunks_desirialize(_out, _in, [](auto& out, auto& in, auto identify)
		{
			if (identify == "SOURCE")
			{
				return	deserialize(out.source, in) ? 1 : 0;
			}
			if (identify == "EVENTS")
			{
				return	deserialize(out.events, in) ? 1 : 0;
			}
			return	-1;
		});
		if (!r)
		{
			return	0;
		}
	}

	return	-1;
}


void schedule_downloader::save()
{
	AMTRS_DEBUG_LOG("Save schedule start");


	std::ostringstream	file;

	std::ostringstream	ss;
	// カレンダー
	ss	= std::ostringstream();
	if (!serialize(ss, gCalendars)) { return; }
	if (!write_chunk(file, "CALENDARLIST", ss.str())) { return; }
	// 終端
	if (!write_chunk(file, "", "")) { return; }

	writefile(filename(), file.str());


	AMTRS_DEBUG_LOG("Save schedule end");
}




bool load_items()
{
	gCalendars		= {};

	std::string	data;
	if (!readfile(data, filename()))
	{
		AMTRS_DEBUG_LOG("cache no load");
		return	false;
	}

	std::istringstream		in(data);
	std::list<calendardata>	calendars;
	auto	r	= subchunks_desirialize(calendars, (std::istream&)in, [&](auto& out, auto& in, auto& identify)
				{
					if (identify == "CALENDARLIST")
					{
						return	deserialize(out, in) ? 1 : 0;
					}
					return	1;	// 知らないデータは無視
				}) ? 1 : 0;
	if (r)
	{
		gCalendars		= std::move(calendars);
		chrono::calendar::enumrate(0, [&](auto _calendar)
		{
			auto	m	= std::find_if(gCalendars.begin(), gCalendars.end(), [&](auto& c)
						{
							return	_calendar->identify() == c.source.identify;
						});
			if (m != gCalendars.end())
			{
				m->source.calendar	= _calendar;
			}
		});
	}
	return	r;
}
