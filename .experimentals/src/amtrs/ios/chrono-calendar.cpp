/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#import <EventKit/EventKit.h>
#include <ctime>
#include <amtrs/chrono/calendar.hpp>
#include <amtrs/chrono/time.hpp>
#include <amtrs/.driver/ios/applekit/string.hpp>
AMTRS_CHRONO_NAMESPACE_BEGIN


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
		AMTRS_DEBUG_LOG("calendar_::events(%s) : isSubscribed:%s", std::to_string(mCalendar.title).c_str(), (mCalendar.isSubscribed ? "YES" : "NO"));
		NSMutableArray*	cals	= [[NSMutableArray alloc] init];
		[cals addObject:mCalendar];
		auto			store		= [[EKEventStore alloc] init];
		auto			begin		= [NSDate dateWithTimeIntervalSince1970:((std::time_t)time(nullptr) - 60*24*60*60)];
		auto			end			= [NSDate dateWithTimeIntervalSince1970:(std::time_t)time(nullptr)];
		NSPredicate*	predicate	= [store predicateForEventsWithStartDate:begin endDate:end calendars:cals];
		for (EKEvent* event : [store eventsMatchingPredicate:predicate])
		{
			calendar::event	e;
			e.identify			= std::to_string(event.eventIdentifier);
			e.title				= std::to_string(event.title);
			e.description		= "";
			e.start				= chrono::timev((std::time_t)[event.startDate timeIntervalSince1970]);
			e.end				= chrono::timev((std::time_t)[event.endDate timeIntervalSince1970]);

			#if		defined(AMTRS_SYSTRACE_LOG_USE)
				auto	bdays	= e.start.days();
				auto	btimes	= e.start.times();
				auto	edays	= e.end.days();
				auto	etimes	= e.end.times();
				AMTRS_SYSTRACE_LOG("Event %s %s %d/%02d/%02d %02d:%02d - %d/%02d/%02d %02d:%02d", e.identify.c_str(), e.title.c_str(), bdays.year, bdays.mon, bdays.day, btimes.hour, btimes.min, edays.year, edays.mon, edays.day, etimes.hour, etimes.min);
			#endif
			_callback(e);
		}
	}


	virtual std::string insert(event const& _e) override
	{
		auto	dtstart	= _e.start;
		auto	dtend	= _e.end;
		if (dtend.get() <= (dtstart.get() + 60 * 60))
		{
			dtend	= timev(dtstart.get() + 60 * 60);
		}
		NSError*	err	= nullptr;
		EKEvent*	ev	= [[EKEvent alloc] initWithEventStore:mStore];
		ev.title		= [NSString stringWithUTF8String:_e.title.c_str()];
		ev.startDate	= [NSDate dateWithTimeIntervalSince1970:(std::time_t)dtstart.get()];
		ev.endDate		= [NSDate dateWithTimeIntervalSince1970:(std::time_t)dtend.get()];
		ev.availability	= EKEventAvailabilityFree;
		ev.calendar		= mCalendar;
		[mStore saveEvent:ev span:EKSpanThisEvent commit:YES error:&err];

		if (err)
		{
			auto	message	= std::to_string([err localizedDescription]);
			AMTRS_WARN_LOG("Insert error %s", message.c_str());
		}

		return	std::to_string(ev.eventIdentifier);
	}


	virtual bool update(event const& _e) override
	{
		auto	dtstart	= _e.start;
		auto	dtend	= _e.end;
		if (dtend.get() <= (dtstart.get() + 60 * 60))
		{
			dtend	= timev(dtstart.get() + 60 * 60);
		}
		EKEvent*	ev	= [mStore eventWithIdentifier:[NSString stringWithUTF8String:_e.identify.c_str()]]; 
        if (ev)
		{
			NSError*	err	= nullptr;
			ev.title			= [NSString stringWithUTF8String:_e.title.c_str()];
			ev.startDate		= [NSDate dateWithTimeIntervalSince1970:(std::time_t)dtstart.get()];
			ev.endDate			= [NSDate dateWithTimeIntervalSince1970:(std::time_t)dtend.get()];
			ev.availability		= EKEventAvailabilityFree;
			ev.calendar			= mCalendar;
			[mStore saveEvent:ev span:EKSpanThisEvent commit:YES error:&err];
			return	true;
		}
		return	false;
	}


	virtual bool erase(std::string const& _identify) override
	{
		EKEvent*	ev	= [mStore eventWithIdentifier:[NSString stringWithUTF8String:_identify.c_str()]]; 
        if (ev)
		{
			NSError*	err	= nullptr;
			[mStore removeEvent:ev span:EKSpanThisEvent commit:YES error:&err];
			return	true;
		}
		return	false;
	}

	virtual bool writable() const override
	{
		return	true;
	}

	EKEventStore*	mStore		= nil;
	EKCalendar*		mCalendar	= nil;
	std::string		mIdentify;
	std::string		mName;
	std::string		mAccount;
	std::string		mOwner;
	rgb<uint8_t>	mColor;
};

/*
ref<calendar> calendar::create(std::string _identify)
{
	EKEventStore*	store	= [[EKEventStore alloc] init];
	for (EKSource* source in store.sources)
	{
		ref<calendar>	retval;
	}
	return	{};
}
*/

void calendar::enumrate(unsigned int _flags, std::function<void(ref<calendar> _calendar)> _callback)
{
	AMTRS_SYSTRACE_LOG("calendar enumrate");
	EKEventStore*	store	= [[EKEventStore alloc] init];
	for (EKSource* source in store.sources)
	{
		if (source.sourceType != EKSourceTypeCalDAV)
		{
			continue;
		}

		for (EKCalendar* c : [source calendarsForEntityType:EKEntityTypeEvent])
		{
			ref<calendar_>	cal	= new calendar_();
			cal->mStore		= store;
			cal->mCalendar	= c;
			cal->mIdentify	= std::to_string(c.calendarIdentifier);
			cal->mName		= std::to_string(c.title);
			cal->mAccount	= "";
			cal->mOwner		= "";
			cal->mColor		= rgb<uint8_t>(0, 0, 0);
			_callback(cal);
		}
	}
}

	
void calendar::permission(std::function<void(bool _enable)> _callback)
{
AMTRS_SYSTRACE_LOG("calendar permission check");
	EKAuthorizationStatus	status	= [EKEventStore authorizationStatusForEntityType:EKEntityTypeEvent];

	// 承認済みかチェック
	if (status == EKAuthorizationStatusAuthorized)
	{
		AMTRS_SYSTRACE_LOG("calendar permission allow");
		_callback(true);
AMTRS_SYSTRACE_LOG("calendar callback finish");
		return;
	}

	// 拒否状態
	if (status == EKAuthorizationStatusDenied || status == EKAuthorizationStatusRestricted)
	{
		AMTRS_SYSTRACE_LOG("calendar permission deny");
		_callback(false);
AMTRS_SYSTRACE_LOG("calendar callback finish");
		return;
	}

	// 許可を求める
	EKEventStore*	store = [[EKEventStore alloc] init];
	[store requestAccessToEntityType:EKEntityTypeEvent completion:^(BOOL granted, NSError* error)
	{
		AMTRS_SYSTRACE_LOG("calendar permission granted = %s", (granted ? "YES" : "NO"));
		_callback(granted);
AMTRS_SYSTRACE_LOG("calendar callback finish");
		return;
	}];
AMTRS_SYSTRACE_LOG("calendar permission exit");
}


AMTRS_CHRONO_NAMESPACE_END
