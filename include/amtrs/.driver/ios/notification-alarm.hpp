/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__notification__alarm__ios__hpp
#define	__libamtrs__notification__alarm__ios__hpp
#import <UserNotifications/UserNotifications.h>
#include "../../logging/@"
AMTRS_NAMESPACE_BEGIN

// ios : UNNotificationRequest

class	alarm_manager_ios
		: public alarm_manager
{
public:
	bool	mAuthorized	= false;

	void initialize(const std::string& _receiverClassName)
	{
		auto*	center		= [UNUserNotificationCenter currentNotificationCenter]; 
		[center
			requestAuthorizationWithOptions:	(UNAuthorizationOptionAlert + UNAuthorizationOptionSound) 
			completionHandler:					^(BOOL _granted, NSError* _Nullable _error)
			{
				mAuthorized	= _granted;
				AMTRS_DEBUG_LOG("Notification auth %s", (mAuthorized ? "true" : "false"));
			}
		];
	}


	virtual void add_alarm(const alarm& _notificate) override
	{
		if (!mAuthorized)
		{
			AMTRS_DEBUG_LOG("No authrozed alarm");
			return;
		}
		UNMutableNotificationContent*		content	= nullptr;
		UNCalendarNotificationTrigger*		trigger	= nullptr;
		UNNotificationRequest*				request	= nullptr;

		// 通知内容
		content			= [[UNMutableNotificationContent alloc] init];
		content.title	= [[NSString alloc] initWithUTF8String:_notificate.title.c_str()];
		content.body	= [[NSString alloc] initWithUTF8String:_notificate.message.c_str()];
		content.sound	= [UNNotificationSound defaultSound];

		// 通知する時間
		auto				lt		= _notificate.time.localtime();
		NSDateComponents*	date	= [[NSDateComponents alloc] init]; 
		date.year		= lt.tm_year + 1900;
		date.month		= lt.tm_mon + 1;
		date.day		= lt.tm_mday;
		date.hour		= lt.tm_hour;
		date.minute		= lt.tm_min;
		date.second		= 0;
		trigger			= [UNCalendarNotificationTrigger triggerWithDateMatchingComponents:date repeats:NO];


		request			= [UNNotificationRequest requestWithIdentifier: @"alarm" content: content trigger: trigger];

		AMTRS_DEBUG_LOG("add alarm %d/%d/%d %02d:%02d:%02d", (int)date.year, (int)date.month, (int)date.day, (int)date.hour, (int)date.minute, (int)date.second);

		auto	center	= [UNUserNotificationCenter currentNotificationCenter];
		[center addNotificationRequest:request
			withCompletionHandler:^(NSError* _Nullable _error)
			{
#if	AMTRS_DEBUG_LOG
				if (!_error)
				{
					NSString*	message	= [_error localizedDescription];
					AMTRS_DEBUG_LOG("Notification error : %s", [message UTF8String]);
				}
				else
				{
					AMTRS_DEBUG_LOG("Notification register : no error");
				}
#endif
			}
		];
	/*
			UNUserNotificationCenter*	center	= [UNUserNotificationCenter currentNotificationCenter];
			center.delegate	= self;
			[center requestAuthorizationWithOptions:(UNAuthorizationOptionSound | UNAuthorizationOptionAlert | UNAuthorizationOptionBadge) completionHandler:^(BOOL granted, NSError * _Nullable error){
				if(!error){
					[[UIApplication sharedApplication] registerForRemoteNotifications];
				}
			}];

	*/
	//		UNUserNotificationCenter*			notif	= nullptr;
	//		notif			= [UNUserNotificationCenter requestAuthorizationWithOptions];
	//		[notif addNotificationRequest:request];
	}


	virtual void cancel_all() override
	{
	}



protected:
	std::string			mReceiverClassName;

	friend	alarm_manager;
};


inline ref<alarm_manager> alarm_manager::create(const std::string& _receiverClassName)
{
	ref<alarm_manager_ios>	thiz	= new alarm_manager_ios();
	thiz->initialize(_receiverClassName);
	return	thiz;
}

AMTRS_NAMESPACE_END
#endif
