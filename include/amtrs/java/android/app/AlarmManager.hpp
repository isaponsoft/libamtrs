/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_app__AlarmManager__hpp
#define	__libamtrs__android__java_classes__android_app__AlarmManager__hpp
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::os {
struct	Handler;
}

namespace android::app {
struct	PendingIntent;


AMTRS_JAVA_DEFINE_CLASS(AlarmManager, java::lang::Object)
{
	using	PendingIntent	= android::app::PendingIntent;
	using	String			= java::lang::String;
	using	Handler			= android::os::Handler;

	AMTRS_JAVA_CLASS_SIGNATURE("android/app/AlarmManager");


	AMTRS_JAVA_DEFINE_CLASS(AlarmClockInfo, java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/app/AlarmManager$AlarmClockInfo");


		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;
		};


		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;

		};
	};



	AMTRS_JAVA_DEFINE_CLASS(OnAlarmListener, java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/app/AlarmManager$OnAlarmListener");


		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;
		};


		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;

		};
	};



	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(String,		ACTION_NEXT_ALARM_CLOCK_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(jint,		ELAPSED_REALTIME)
		AMTRS_JAVA_DEFINE_FIELD(jint,		ELAPSED_REALTIME_WAKEUP)
		AMTRS_JAVA_DEFINE_FIELD(jlong,		INTERVAL_DAY)
		AMTRS_JAVA_DEFINE_FIELD(jlong,		INTERVAL_FIFTEEN_MINUTES)
		AMTRS_JAVA_DEFINE_FIELD(jlong,		INTERVAL_HALF_DAY)
		AMTRS_JAVA_DEFINE_FIELD(jlong,		INTERVAL_HALF_HOUR)
		AMTRS_JAVA_DEFINE_FIELD(jlong,		INTERVAL_HOUR)
		AMTRS_JAVA_DEFINE_FIELD(jint,		RTC)
		AMTRS_JAVA_DEFINE_FIELD(jint,		RTC_WAKEUP)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(cancel
			, void(PendingIntent)
			, void(OnAlarmListener listener)
		)

		AMTRS_JAVA_DEFINE_METHOD(getNextAlarmClock
			, AlarmClockInfo()
		)

		AMTRS_JAVA_DEFINE_METHOD(set
			, void(jint type, jlong triggerAtMillis, PendingIntent operation)
			, void(jint type, jlong triggerAtMillis, String tag, OnAlarmListener listener, Handler targetHandler)
		)

		AMTRS_JAVA_DEFINE_METHOD(setAlarmClock
			, void(AlarmClockInfo info, PendingIntent operation)
		)

		AMTRS_JAVA_DEFINE_METHOD(setAndAllowWhileIdle
			, void(jint type, jlong triggerAtMillis, PendingIntent operation)
		)

		AMTRS_JAVA_DEFINE_METHOD(setExact
			, void(jint type, jlong triggerAtMillis, PendingIntent operation)
			, void(jint type, jlong triggerAtMillis, String tag, OnAlarmListener listener, Handler targetHandler)
		)

		AMTRS_JAVA_DEFINE_METHOD(setExactAndAllowWhileIdle
			, void(jint type, jlong triggerAtMillis, PendingIntent operation)
		)

		AMTRS_JAVA_DEFINE_METHOD(setInexactRepeating
			, void(jint type, jlong triggerAtMillis, jlong intervalMillis, PendingIntent operation)
		)

		AMTRS_JAVA_DEFINE_METHOD(setRepeating
			, void(jint type, jlong triggerAtMillis, jlong intervalMillis, PendingIntent operation)
		)

		AMTRS_JAVA_DEFINE_METHOD(setTime
			, void(jlong millis)
		)

		AMTRS_JAVA_DEFINE_METHOD(setTimeZone
			, void(String timeZone)
		)

		AMTRS_JAVA_DEFINE_METHOD(setWindow
			, void(jint type, jlong windowStartMillis, jlong windowLengthMillis, PendingIntent operation)
			, void(jint type, jlong windowStartMillis, jlong windowLengthMillis, String tag, OnAlarmListener listener, Handler targetHandler)
		)

	};

};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
