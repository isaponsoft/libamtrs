/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__provider__CalenderContract__hpp
#define	__libamtrs__android__java_classes__android__provider__CalenderContract__hpp
#include "../../java/lang/String.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::content {
struct	Context;
struct	ContentResolver;
}
namespace android::database {
struct	Cursor;
}
namespace android::net {
struct	Uri;
}
namespace android::provider {


AMTRS_JAVA_DEFINE_CLASS(CalendarContract, java::lang::Object)
{
	using	Context	= android::content::Context;
	using	String	= java::lang::String;
	using	Uri		= android::net::Uri;

	AMTRS_JAVA_CLASS_SIGNATURE("android/provider/CalendarContract");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(String, ACCOUNT_TYPE_LOCAL)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_EVENT_REMINDER)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_HANDLE_CUSTOM_EVENT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_VIEW_MANAGED_PROFILE_CALENDAR_EVENT)
		AMTRS_JAVA_DEFINE_FIELD(String, AUTHORITY)
		AMTRS_JAVA_DEFINE_FIELD(String, CALLER_IS_SYNCADAPTER)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_CUSTOM_APP_URI)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_EVENT_ALL_DAY)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_EVENT_BEGIN_TIME)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_EVENT_END_TIME)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_EVENT_ID)
		AMTRS_JAVA_DEFINE_FIELD(Uri, 	CONTENT_URI)


		AMTRS_JAVA_DEFINE_METHOD(
			startViewCalendarEventInManagedProfile,
			jboolean(Context context, jlong eventId, jlong startMs, jlong endMs, jboolean allDay, jint flags)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;
	};



	AMTRS_JAVA_DEFINE_CLASS(Calendars, java::lang::Object)
	{
		using	String	= java::lang::String;
		using	Uri		= android::net::Uri;

		AMTRS_JAVA_CLASS_SIGNATURE("android/provider/CalendarContract$Calendars");


		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;

			AMTRS_JAVA_DEFINE_FIELD(String, _ID)
			AMTRS_JAVA_DEFINE_FIELD(String, CALENDAR_LOCATION)
			AMTRS_JAVA_DEFINE_FIELD(String, DEFAULT_SORT_ORDER)
			AMTRS_JAVA_DEFINE_FIELD(String, NAME)
			AMTRS_JAVA_DEFINE_FIELD(Uri,	CONTENT_URI)
			AMTRS_JAVA_DEFINE_FIELD(Uri,	ENTERPRISE_CONTENT_URI)
		};


		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;
		};
	};



	AMTRS_JAVA_DEFINE_CLASS(Events, java::lang::Object)
	{
		using	String	= java::lang::String;
		using	Uri		= android::net::Uri;

		AMTRS_JAVA_CLASS_SIGNATURE("android/provider/CalendarContract$Events");


		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;

			AMTRS_JAVA_DEFINE_FIELD(Uri,	CONTENT_EXCEPTION_URI)
			AMTRS_JAVA_DEFINE_FIELD(Uri,	CONTENT_URI)
			AMTRS_JAVA_DEFINE_FIELD(Uri,	ENTERPRISE_CONTENT_URI)
		};


		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;
		};
	};



	AMTRS_JAVA_DEFINE_CLASS(Instances, java::lang::Object)
	{
		using	ContentResolver	= android::content::ContentResolver;
		using	Cursor			= android::database::Cursor;
		using	String			= java::lang::String;
		using	Uri				= android::net::Uri;

		AMTRS_JAVA_CLASS_SIGNATURE("android/provider/CalendarContract$Instances");


		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;

			AMTRS_JAVA_DEFINE_FIELD(String,	BEGIN)
			AMTRS_JAVA_DEFINE_FIELD(String,	END)
			AMTRS_JAVA_DEFINE_FIELD(String,	END_DAY)
			AMTRS_JAVA_DEFINE_FIELD(String,	END_MINUTE)
			AMTRS_JAVA_DEFINE_FIELD(String,	EVENT_ID)
			AMTRS_JAVA_DEFINE_FIELD(String,	START_DAY)
			AMTRS_JAVA_DEFINE_FIELD(String,	START_MINUTE)

			AMTRS_JAVA_DEFINE_FIELD(Uri,	CONTENT_BY_DAY_URI)
			AMTRS_JAVA_DEFINE_FIELD(Uri,	CONTENT_SEARCH_BY_DAY_URI)
			AMTRS_JAVA_DEFINE_FIELD(Uri,	CONTENT_SEARCH_URI)
			AMTRS_JAVA_DEFINE_FIELD(Uri,	CONTENT_URI)
			AMTRS_JAVA_DEFINE_FIELD(Uri,	ENTERPRISE_CONTENT_BY_DAY_URI)
			AMTRS_JAVA_DEFINE_FIELD(Uri,	ENTERPRISE_CONTENT_SEARCH_BY_DAY_URI)
			AMTRS_JAVA_DEFINE_FIELD(Uri,	ENTERPRISE_CONTENT_SEARCH_URI)
			AMTRS_JAVA_DEFINE_FIELD(Uri,	ENTERPRISE_CONTENT_URI)
		};


		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;


			AMTRS_JAVA_DEFINE_METHOD(
				query,
				Cursor(ContentResolver cr, String projection[], jlong begin, jlong end),
				Cursor(ContentResolver cr, String projection[], jlong begin, jlong end, String searchQuery)
			)
		};
	};

};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
