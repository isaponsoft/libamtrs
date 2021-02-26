/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/notification/@>
#include <timerapp-core/manager/notification.hpp>
#include <timerapp-core/save/storage.hpp>
#include <timerapp-core/application.hpp>

using namespace amtrs;

static	ref<alarm_manager>	gAlarm;

void alarm_notification_manager::clean()
{
	AMTRS_DEBUG_LOG("Clean notification");
	gAlarm	= amtrs::alarm_manager::create(get_application_config()->alarm_package_name);
	gAlarm->cancel_all();
}


void alarm_notification_manager::setup()
{
	AMTRS_DEBUG_LOG("Setup notification");
	if (gAlarm)
	{
		if (!save::storage::current()->setting.alarm_disable())
		{
			int		id	= 0;
			auto	now	= save::timetype::now();
			for (auto& s : save::storage::current()->schedules)
			{
				if (!s->status.notify()) 		continue;		// 通知無効
				if (s->time.alarmTime <= now)	continue;		// 過去の時間
				amtrs::alarm_manager::alarm	data;

				data.groupId	= "alarm";
				data.groupTitle	= "予定の通知";

				data.id		= ++id;
				data.title		= s->basic.title;
				data.message	= locale::gettext("notification.message");	
				data.time		= s->time.alarmTime.get();
				data.wakeup		= s->status.wakeup() ? 1 : 0;
				AMTRS_DEBUG_LOG("%d: %s, wakeup:%s", (int)data.id, data.title.c_str(), data.wakeup ? "ON" : "OFF");
				gAlarm->add_alarm(data);
			}
		}
	}
	gAlarm.clear();
}

