/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__notification__alarm_android__hpp
#define	__libamtrs__notification__alarm_android__hpp
#include "../java/android/app/Activity.hpp"
#include "../java/android/app/AlarmManager.hpp"
#include "../java/android/app/PendingIntent.hpp"
#include "../java/android/content/Intent.hpp"
#include "../java/android/content/BroadcastReceiver.hpp"
#include "../java/jp/libamtrs/alarm/AlarmReceiver.hpp"
AMTRS_NAMESPACE_BEGIN


//!	1.	AmtrsBroadcastReceiver を継承したクラスを用意
//!	2.	AndroidManifext.xml
//!			<application>
//!				<receiver android:name=".AlermReciver" android:process=":remote" />
//!			</application>
//!		を追加
class	alarm_manager_android
		: public alarm_manager
{
public:
	using	Activity		= java::classes::android::app::Activity;
	using	Class			= java::classes::java::lang::Class;
	using	String			= java::classes::java::lang::String;
	using	Context			= java::classes::android::content::Context;
	using	Intent			= java::classes::android::content::Intent;
	using	AlarmManager	= java::classes::android::app::AlarmManager;
	using	PendingIntent	= java::classes::android::app::PendingIntent;
	using	AlarmReceiver	= java::classes::jp::libamtrs::alarm::AlarmReceiver;

	void initialize(const std::string& _receiverClassName)
	{
		mReceiverClassName	= _receiverClassName;
		mReceiverClass		= java::local_cls::make((jclass)java::get_jnienv()->FindClass(mReceiverClassName.c_str()));
	}

	
	virtual void add_alarm(const alarm& _notificate) override
	{
		AlarmReceiver::find().addAlarm(
			java::jobj<Activity>(os::android::get_activity()),
			java::jobj<Class>(mReceiverClass),
			((jlong)_notificate.time.unixtime() * 1000),
			_notificate.id,
			java::jobj<String>(java::to_jstring(_notificate.title)),
			java::jobj<String>(java::to_jstring(_notificate.message))
		);
	}


	virtual void cancel_all() override
	{
		auto	cls		= java::jobj<Class>::make((jclass)java::get_jnienv()->FindClass(mReceiverClassName.c_str()));

		AlarmReceiver::find().cancelAll(
			java::jobj<Activity>(os::android::get_activity()),
			java::jobj<Class>(mReceiverClass)
		);
	}



protected:
	std::string			mReceiverClassName;
	java::global_cls	mReceiverClass;

	friend	alarm_manager;
};


inline ref<alarm_manager> alarm_manager::create(const std::string& _receiverClassName)
{
	ref<alarm_manager_android>	thiz	= new alarm_manager_android();
	thiz->initialize(_receiverClassName);
	return	thiz;
}



AMTRS_NAMESPACE_END
#endif
