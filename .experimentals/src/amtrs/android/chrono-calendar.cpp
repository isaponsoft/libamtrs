/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/chrono.hpp>
#include <amtrs/java/android/Manifest.hpp>
#include <amtrs/java/android/accounts/Account.hpp>
#include <amtrs/java/android/accounts/AccountManager.hpp>
#include <amtrs/java/android/database/Cursor.hpp>
#include <amtrs/java/android/net/Uri.hpp>
#include <amtrs/java/android/provider/CalendarContract.hpp>
#include <amtrs/java/android/content/ContentProviderClient.hpp>
#include <amtrs/java/android/content/ContentResolver.hpp>
#include <amtrs/java/android/content/ContentUris.hpp>
#include <amtrs/java/android/content/ContentValues.hpp>
#include <amtrs/java/android/database/ContentObserver.hpp>
#include <amtrs/java/android/os/Bundle.hpp>
#include <amtrs/java/java/util/TimeZone.hpp>
#include <amtrs/.driver/android-api-android_app.hpp>
#include <amtrs/.driver/android-api-permissions.hpp>
AMTRS_CHRONO_NAMESPACE_BEGIN

using	Account					= java::classes::android::accounts::Account;
using	AccountManager			= java::classes::android::accounts::AccountManager;
using	Bundle					= java::classes::android::os::Bundle;
using	ContentValues			= java::classes::android::content::ContentValues;
using	ContentUris				= java::classes::android::content::ContentUris;
using	ContentProviderClient	= java::classes::android::content::ContentProviderClient;
using	ContentResolver			= java::classes::android::content::ContentResolver;
using	CalendarContract		= java::classes::android::provider::CalendarContract;
using	Calendars				= CalendarContract::Calendars;
using	Events					= CalendarContract::Events;
using	String					= java::classes::java::lang::String;
using	TimeZone				= java::classes::java::util::TimeZone;

class	calendar_
		: public calendar
{
public:
	virtual std::string identify() const override
	{
		return	mIdentify;
	}


	virtual std::string name() const override
	{
		return	mName;
	}


	virtual std::string account() const override
	{
		return	mAccount;
	}


	virtual std::string owner_account() const override
	{
		return	mOwner;
	}


	virtual rgb<uint8_t> color() const override
	{
		return	mColor;
	}


	virtual void events(std::function<void(event const&)> _callback) override
	{
		auto			resolver	= os::android::get_activity().getContentResolver().acquireContentProviderClient(CalendarContract::find().get_CONTENT_URI());
		
		jclass			cls			= java::get_jnienv()->FindClass("java/lang/String");
		jobjectArray	ary			= java::get_jnienv()->NewObjectArray(8, cls, java::get_jnienv()->NewStringUTF(""));
		java::get_jnienv()->SetObjectArrayElement(ary, 0, java::to_jstring("_id"));
		java::get_jnienv()->SetObjectArrayElement(ary, 1, java::to_jstring("deleted"));
		java::get_jnienv()->SetObjectArrayElement(ary, 2, java::to_jstring("title"));
		java::get_jnienv()->SetObjectArrayElement(ary, 3, java::to_jstring("description"));
		java::get_jnienv()->SetObjectArrayElement(ary, 4, java::to_jstring("dtstart"));
		java::get_jnienv()->SetObjectArrayElement(ary, 5, java::to_jstring("dtend"));
		java::get_jnienv()->SetObjectArrayElement(ary, 6, java::to_jstring("availability"));
		java::get_jnienv()->SetObjectArrayElement(ary, 7, java::to_jstring("eventTimezone"));


		auto			selection	= java::to_jstring("(calendar_id = ?)");
		jobjectArray	selectArgs	= java::get_jnienv()->NewObjectArray(1, cls, java::get_jnienv()->NewStringUTF(""));
		java::get_jnienv()->SetObjectArrayElement(selectArgs, 0, java::to_jstring(mIdentify));

		auto	cur	= resolver.query(
						java::classes::android::provider::CalendarContract::Events::find().get_CONTENT_URI(),
						java::jobj<java::classes::java::lang::String[]>(ary),
						java::jobj<java::classes::java::lang::String>(selection.get()),	// selection
						java::jobj<java::classes::java::lang::String[]>(selectArgs),	// selectionArgs,
						java::jobj<java::classes::java::lang::String>()					// sortOder
					);
		java::get_jnienv()->DeleteLocalRef(ary);
		java::get_jnienv()->DeleteLocalRef(selectArgs);
		if (cur.moveToFirst())
		{
			do
			{
				auto	del		= std::to_string((jstring)cur.getString(1).get());
				if (del != "0")
				{
					continue;
				}

				event	e;

				e.identify			= std::to_string((jstring)cur.getString(0).get());
				e.title				= std::to_string((jstring)cur.getString(2).get());
				e.description		= std::to_string((jstring)cur.getString(3).get());
				e.start				= chrono::timev((std::time_t)(cur.getLong(4)/1000));
				e.end				= chrono::timev((std::time_t)(cur.getLong(5)/1000));


			#if		defined(AMTRS_SYSTRACE_LOG_USE)
				auto	bdays	= e.start.days();
				auto	btimes	= e.start.times();
				auto	edays	= e.end.days();
				auto	etimes	= e.end.times();
				AMTRS_SYSTRACE_LOG("Event %s %s %d/%02d/%02d %02d:%02d - %d/%02d/%02d %02d:%02d", e.identify.c_str(), e.title.c_str(), bdays.year, bdays.mon, bdays.day, btimes.hour, btimes.min, edays.year, edays.mon, edays.day, etimes.hour, etimes.min);
			#endif
				_callback(e);
			} while (cur.moveToNext());
		}
		cur.close();
		resolver.close();
	}


	virtual std::string insert(event const& _e) override
	{
		auto	resolver	= os::android::get_activity().getContentResolver().acquireContentProviderClient(CalendarContract::find().get_CONTENT_URI());
		auto	dtstart	= _e.start;
		auto	dtend	= _e.end;
		if (dtend.get() <= (dtstart.get() + 60 * 60))
		{
			dtend	= timev(dtstart.get() + 60 * 60);
		}
		auto	bdays	= dtstart.days();
		auto	btimes	= dtstart.times();
		auto	edays	= dtend.days();
		auto	etimes	= dtend.times();
		AMTRS_SYSTRACE_LOG("Calendar insert %s %s %d/%02d/%02d %02d:%02d - %d/%02d/%02d %02d:%02d", _e.identify.c_str(), _e.title.c_str(), bdays.year, bdays.mon, bdays.day, btimes.hour, btimes.min, edays.year, edays.mon, edays.day, etimes.hour, etimes.min);


		auto	tz		= TimeZone::find().getDefault().getID();
		// 書き込む値
		auto		values	= ContentValues::find().init();
		values.put(java::jobj<String>(java::to_jstring("calendar_id")),		java::jobj<String>(java::to_jstring(mIdentify)));
		values.put(java::jobj<String>(java::to_jstring("title")),			java::jobj<String>(java::to_jstring(_e.title)));
		values.put(java::jobj<String>(java::to_jstring("description")),		java::jobj<String>(java::to_jstring(_e.description)));
		values.put(java::jobj<String>(java::to_jstring("dtstart")),		ContentValues::Long::find().init((long)dtstart.get() * 1000));
		values.put(java::jobj<String>(java::to_jstring("dtend")),		ContentValues::Long::find().init((long)dtend.get()   * 1000));
		values.put(java::jobj<String>(java::to_jstring("eventTimezone")),	java::jobj<String>(tz));
		auto		evid	= resolver.insert(Events::find().get_CONTENT_URI(), values);
		notifyChange(evid);
		resolver.close();
		return	std::to_string((jstring)evid.getLastPathSegment().get());
	}


	virtual bool update(event const& _e) override
	{
		auto	dtstart	= _e.start;
		auto	dtend	= _e.end;
		if (dtend.get() <= (dtstart.get() + 60 * 60))
		{
			dtend	= timev(dtstart.get() + 60 * 60);
		}
		auto	bdays	= dtstart.days();
		auto	btimes	= dtstart.times();
		auto	edays	= dtend.days();
		auto	etimes	= dtend.times();
		AMTRS_SYSTRACE_LOG("Calendar update %s %s %d/%02d/%02d %02d:%02d - %d/%02d/%02d %02d:%02d", _e.identify.c_str(), _e.title.c_str(), bdays.year, bdays.mon, bdays.day, btimes.hour, btimes.min, edays.year, edays.mon, edays.day, etimes.hour, etimes.min);


		// 書き込み先のID
		auto		id		= ContentValues::Long::find().init(java::to_jstring(_e.identify)).longValue();
		auto		uri		= ContentUris::find().withAppendedId(Events::find().get_CONTENT_URI(), id);


		auto	tz		= TimeZone::find().getDefault().getID();
		// 書き込む値
		auto		values	= ContentValues::find().init();
		values.put(java::jobj<String>(java::to_jstring("title")),		java::jobj<String>(java::to_jstring(_e.title)));
		values.put(java::jobj<String>(java::to_jstring("description")),	java::jobj<String>(java::to_jstring(_e.description)));
		values.put(java::jobj<String>(java::to_jstring("dtstart")),		ContentValues::Long::find().init((long)dtstart.get() * 1000));
		values.put(java::jobj<String>(java::to_jstring("dtend")),		ContentValues::Long::find().init((long)dtend.get()   * 1000));
		values.put(java::jobj<String>(java::to_jstring("eventTimezone")),	java::jobj<String>(tz));
		auto		rows	= os::android::get_activity().getContentResolver().update(uri, values, java::jobj<String>(), java::jobj<String[]>());
		AMTRS_SYSTRACE_LOG("Update rows = %d", (int)rows);
		notifyChange(uri);
		return	rows != 0;
	}


	virtual bool erase(std::string const& _identify) override
	{
		AMTRS_SYSTRACE_LOG("Delete %s", _identify.c_str());
		auto		id		= ContentValues::Long::find().init(java::to_jstring(_identify)).longValue();
		auto		uri		= ContentUris::find().withAppendedId(Events::find().get_CONTENT_URI(), id);
		auto		rows	= os::android::get_activity().getContentResolver().delete_(uri, java::jobj<String>(), java::jobj<String[]>());
		AMTRS_SYSTRACE_LOG("Delete rows = %d", (int)rows);
		notifyChange(uri);
		return	rows != 0;
	}

	virtual bool writable() const override
	{
		return	true;
	}

	void notifyChange(java::jobj<java::classes::android::net::Uri> _uri)
	{
#if	0
		java::jobj<java::classes::android::database::ContentObserver>	o;

		AMTRS_SYSTRACE_LOG("notifyChange = %s", std::to_string((jstring)_uri.toString().get()).c_str());
		os::android::get_activity().getContentResolver().notifyChange(_uri, o);
		_uri	= java::classes::android::provider::CalendarContract::Events::find().get_CONTENT_URI();
		os::android::get_activity().getContentResolver().notifyChange(_uri, o);


		auto	accounts	= AccountManager::find().get(os::android::get_activity()).getAccounts();
		auto	authority	= CalendarContract::Calendars::find().get_CONTENT_URI().getAuthority();
		for (int i = 0; i < java::get_jnienv()->GetArrayLength(accounts); ++i)
		{
			auto	bundle	= Bundle::find().init();
			bundle.putBoolean(ContentResolver::find().get_SYNC_EXTRAS_MANUAL(), true);
			os::android::get_activity().getContentResolver().requestSync(java::jobj<Account>(java::get_jnienv()->GetObjectArrayElement(accounts, i)), authority, bundle);
		}
#endif
	}

	std::string		mIdentify;
	std::string		mName;
	std::string		mAccount;
	std::string		mOwner;
	rgb<uint8_t>	mColor;
};


void calendar::enumrate(unsigned int _flags, std::function<void(ref<calendar> _calendar)> _callback)
{
	jclass			cls		= java::get_jnienv()->FindClass("java/lang/String");
	jobjectArray	ary		= java::get_jnienv()->NewObjectArray(5, cls, java::get_jnienv()->NewStringUTF(""));
	java::get_jnienv()->SetObjectArrayElement(ary, 0, java::to_jstring("_id"));
	java::get_jnienv()->SetObjectArrayElement(ary, 1, java::to_jstring("name"));
	java::get_jnienv()->SetObjectArrayElement(ary, 2, java::to_jstring("account_name"));
	java::get_jnienv()->SetObjectArrayElement(ary, 3, java::to_jstring("ownerAccount"));
	java::get_jnienv()->SetObjectArrayElement(ary, 4, java::to_jstring("calendar_color"));

	auto	rlv	= os::android::get_activity().getContentResolver();
	auto	cur	= rlv.query(
					java::classes::android::provider::CalendarContract::Calendars::find().get_CONTENT_URI(),
					java::jobj<java::classes::java::lang::String[]>(ary),
					java::jobj<java::classes::java::lang::String>(),		// selection
					java::jobj<java::classes::java::lang::String[]>(),		// selectionArgs,
					java::jobj<java::classes::java::lang::String>()			// sortOder
				);
	java::get_jnienv()->DeleteLocalRef(ary);
	while (cur.moveToNext())
	{
		ref<calendar_>	cal	= new calendar_();
		cal->mIdentify	= std::to_string((jstring)cur.getString(0).get());
		cal->mName		= std::to_string((jstring)cur.getString(1).get());
		cal->mAccount	= std::to_string((jstring)cur.getString(2).get());
		cal->mOwner		= std::to_string((jstring)cur.getString(3).get());
		if ((!(_flags & enumrate_flag::enum_systems)) && cal->mOwner.find("@group.v.calendar.google.com") != std::string::npos)
		{
			continue;
		}
		int			color	= cur.getInt(4);
		cal->mColor		= rgb<uint8_t>((uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)(color));
		_callback(cal);
	}
	cur.close();
}



void calendar::permission(std::function<void(bool _enable)> _callback)
{
	auto	permR	= java::classes::android::Manifest::permission::find().get_READ_CALENDAR();
	auto	permW	= java::classes::android::Manifest::permission::find().get_WRITE_CALENDAR();
	os::android::permission({permR, permW}, [=](bool _enable)
	{
		_callback(_enable);
	});
}

ref<calendar> calendar::create(std::string _identify)
{
	ref<calendar>	retval;
	enumrate(0, [&](auto c)
	{
		if (c->identify() == _identify)
		{
			retval	= c;
		}
	});
	return	retval;
}



AMTRS_CHRONO_NAMESPACE_END
