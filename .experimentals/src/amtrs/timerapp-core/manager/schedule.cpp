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
#include <timerapp-core/application.hpp>
#include <timerapp-core/save/storage.hpp>
#include <timerapp-core/manager/schedule.hpp>
#include <timerapp-core/manager/schedule_downloader.hpp>
namespace schedule_manager {

static void save_to_local(schedule_type const* _schedule);
static schedule_source* find_source(std::string const& _providor);

//! スケジュールデータの同期を行います。
bool sync()
{
	if (schedule_downloader::update_to(save::storage::current()->schedules))
	{
		save::storage::current()->save();
		return	true;
	}
	return	false;
}



void insert(schedule_type const* _schedule)
{
	schedule_source*	source	= nullptr;
	if (_schedule->time.type == save::schedule_type::schedule)
	{
		schedule_downloader::enumrate_source([&](auto s)
		{
			if (_schedule->basic.providor == s->identify)
			{
				source	= s;
			}
		});
	}

	amtrs::ref<schedule_type>	s	= new schedule_type();
	*s	= *_schedule;
	if (source)
	{
		AMTRS_DEBUG_LOG("Save to event source %s", source->identify.c_str());
		amtrs::chrono::calendar::event	e;
		e.title			= s->basic.title;
		e.description	= s->extra.note;
		e.url			= s->extra.url;
		e.start			= s->time.begintime;
		e.end			= s->time.endtime;
		s->basic.identify	= source->calendar->insert(e);
		s->basic.providor	= source->identify;
		AMTRS_DEBUG_LOG("Save to event source %s Identify = %s", s->basic.providor.c_str(), s->basic.identify.c_str());
		schedule_downloader::sync(source->calendar->identify());
	}
	else
	{
		s->basic.identify	= save::storage::current()->create_schedule_identify();
		s->basic.providor	= {};
	}
	save_to_local(s);
	save::storage::current()->setting.event.selectCalendar	= s->basic.providor;
	save::storage::current()->save();
}



void save(schedule_type const* _schedule)
{
	schedule_source*	source	= nullptr;
	if (_schedule->time.type == save::schedule_type::schedule)
	{
		source	= find_source(_schedule->basic.providor);
	}
	if (source)
	{
		amtrs::chrono::calendar::event	e;
		e.identify		= _schedule->basic.identify;
		e.title			= _schedule->basic.title;
		e.description	= _schedule->extra.note;
		e.url			= _schedule->extra.url;
		e.start			= _schedule->time.begintime;
		e.end			= _schedule->time.endtime;
		source->calendar->update(e);
		schedule_downloader::sync(source->calendar->identify());
	}

	// サウンドの設定などアプリ固有のデータもあるため
	// ローカルにも保存する。
	save_to_local(_schedule);
	save::storage::current()->setting.event.selectCalendar	= _schedule->basic.providor;
	save::storage::current()->save();
}



bool erase(schedule_type const* _schedule)
{
	return	erase(_schedule->basic.providor, _schedule->basic.identify);
}



bool erase(std::string const& _providor, std::string const& _identify)
{
	auto&	schedules	= save::storage::current()->schedules;
	auto	it			= std::remove_if(schedules.begin(), schedules.end(), [&](auto& s)
	{
		return	s->basic.providor == _providor
			&&	s->basic.identify == _identify;
	});
	if (it != schedules.end())
	{
		schedules.erase(it);
	}
	schedule_downloader::remove(_providor, _identify);
	save::storage::current()->save();
	auto	source	= find_source(_providor);
	if (source)
	{
		source->calendar->erase(_identify);
		schedule_downloader::sync(source->calendar->identify());
	}
	return	true;
}


//! カレンダーの表示名を取得します。
std::string calendar_display_name(std::string const& _providor)
{
	std::string	name	= locale::gettext("event.source.default");
	schedule_downloader::enumrate_source([&](auto src)
	{
		if (src->identify == _providor)
		{
			name	= src->title;
		}
	});
	return	name;
}


std::string default_calendar_id()
{
	std::string	id;
	schedule_downloader::enumrate_source([&](auto src)
	{
		if (src->identify == save::storage::current()->setting.event.selectCalendar)
		{
			id	= src->identify;
		}
	});
	return	id;
}




// ローカル領域へ保存
void save_to_local(schedule_type const* _schedule)
{
	auto&	schedules	= save::storage::current()->schedules;

	// 同一のアドレスがある場合は直接編集されているので何もしない
	for (auto& s : schedules)
	{
		if (s == _schedule)
		{
			AMTRS_DEBUG_LOG("Direct local write");
			return;
		}
	}

	// 同一のIDが存在する場合は中身を上書きする
	for (auto& s : schedules)
	{
		if (s->basic.providor == _schedule->basic.providor
		 && s->basic.identify == _schedule->basic.identify)
		{
			*s	= *_schedule;
			return;
		}
	}

	// リストに新規追加
	AMTRS_DEBUG_LOG("New local schedule");
	amtrs::ref<schedule_type>	s	= new schedule_type();
	*s	= *_schedule;
	schedules.push_back(std::move(s));
}


schedule_source* find_source(std::string const& _providor)
{
	schedule_source*	retval	= nullptr;
	schedule_downloader::enumrate_source([&](auto source)
	{
		if (source->identify == _providor)
		{
			retval	= source;
		}
	});
	return	retval;
	
}


}
