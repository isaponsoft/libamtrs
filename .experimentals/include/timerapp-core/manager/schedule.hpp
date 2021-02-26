/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__timerapp_core__manager__schedule__hpp
#define	__timerapp_core__manager__schedule__hpp
#include <amtrs/chrono/calendar.hpp>
#include "../save/schedule.hpp"
namespace schedule_manager {

using	schedule_type	= save::schedule;

//! スケジュールデータの同期を行います。
bool sync();


//! 新規にスケジュールを追加します。
//! identifyとprovidor は無視され、新たに設定されます。
void insert(schedule_type const* _schedule);


void save(schedule_type const* _schedule);

bool erase(schedule_type const* _schedule);
bool erase(std::string const& _providor, std::string const& _identify);

//! カレンダーの表示名を取得します。
std::string calendar_display_name(std::string const& _providor);

//! デフォルトの書き込み先のカレンダーIDを取得します。
std::string default_calendar_id();

}
#endif
