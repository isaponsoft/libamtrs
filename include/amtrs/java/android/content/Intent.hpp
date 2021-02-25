﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_content_Intent__hpp
#define	__libamtrs__android__java_classes__android_content_Intent__hpp
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::net {
struct	Uri;
}

namespace android::os {
struct	Bundle;
}

namespace android::content {

struct	Context;

// https://developer.android.com/reference/android/content/Intent.html
AMTRS_JAVA_DEFINE_CLASS(Intent , java::lang::Object)
{
	using	Class				= java::lang::Class;
	using	Bundle				= android::os::Bundle;
	using	Uri					= android::net::Uri;
	using	String				= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/content/Intent");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_AIRPLANE_MODE_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_ALL_APPS)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_ANSWER)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_APPLICATION_PREFERENCES)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_APPLICATION_RESTRICTIONS_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_APP_ERROR)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_ASSIST)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_ATTACH_DATA)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_BATTERY_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_BATTERY_LOW)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_BATTERY_OKAY)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_BOOT_COMPLETED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_BUG_REPORT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_CALL)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_CALL_BUTTON)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_CAMERA_BUTTON)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_CARRIER_SETUP)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_CHOOSER)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_CLOSE_SYSTEM_DIALOGS)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_CONFIGURATION_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_CREATE_DOCUMENT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_CREATE_SHORTCUT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_DATE_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_DEFAULT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_DELETE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_DEVICE_STORAGE_LOW)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_DEVICE_STORAGE_OK)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_DIAL)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_DOCK_EVENT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_DREAMING_STARTED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_DREAMING_STOPPED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_EDIT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_EXTERNAL_APPLICATIONS_AVAILABLE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_FACTORY_TEST)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_GET_CONTENT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_GET_RESTRICTION_ENTRIES)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_GTALK_SERVICE_CONNECTED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_GTALK_SERVICE_DISCONNECTED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_HEADSET_PLUG)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_INPUT_METHOD_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_INSERT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_INSERT_OR_EDIT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_INSTALL_FAILURE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_INSTALL_PACKAGE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_LOCALE_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_LOCKED_BOOT_COMPLETED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MAIN)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MANAGED_PROFILE_ADDED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MANAGED_PROFILE_AVAILABLE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MANAGED_PROFILE_REMOVED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MANAGED_PROFILE_UNAVAILABLE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MANAGED_PROFILE_UNLOCKED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MANAGE_NETWORK_USAGE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MANAGE_PACKAGE_STORAGE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MEDIA_BAD_REMOVAL)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MEDIA_BUTTON)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MEDIA_CHECKING)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MEDIA_EJECT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MEDIA_MOUNTED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MEDIA_NOFS)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MEDIA_REMOVED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MEDIA_SCANNER_FINISHED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MEDIA_SCANNER_SCAN_FILE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MEDIA_SCANNER_STARTED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MEDIA_SHARED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MEDIA_UNMOUNTABLE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MEDIA_UNMOUNTED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MY_PACKAGE_REPLACED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_NEW_OUTGOING_CALL)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_OPEN_DOCUMENT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_OPEN_DOCUMENT_TREE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PACKAGES_SUSPENDED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PACKAGES_UNSUSPENDED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PACKAGE_ADDED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PACKAGE_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PACKAGE_DATA_CLEARED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PACKAGE_FIRST_LAUNCH)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PACKAGE_FULLY_REMOVED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PACKAGE_INSTALL)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PACKAGE_NEEDS_VERIFICATION)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PACKAGE_REMOVED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PACKAGE_REPLACED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PACKAGE_RESTARTED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PACKAGE_VERIFIED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PASTE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PICK)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PICK_ACTIVITY)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_POWER_CONNECTED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_POWER_DISCONNECTED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_POWER_USAGE_SUMMARY)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PROCESS_TEXT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_PROVIDER_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_QUICK_CLOCK)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_QUICK_VIEW)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_REBOOT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_RUN)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_SCREEN_OFF)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_SCREEN_ON)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_SEARCH)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_SEARCH_LONG_PRESS)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_SEND)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_SENDTO)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_SEND_MULTIPLE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_SET_WALLPAPER)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_SHOW_APP_INFO)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_SHUTDOWN)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_SYNC)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_SYSTEM_TUTORIAL)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_TIMEZONE_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_TIME_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_TIME_TICK)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_UID_REMOVED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_UMS_CONNECTED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_UMS_DISCONNECTED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_UNINSTALL_PACKAGE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_USER_BACKGROUND)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_USER_FOREGROUND)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_USER_INITIALIZE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_USER_PRESENT)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_USER_UNLOCKED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_VIEW)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_VOICE_COMMAND)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_WALLPAPER_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_WEB_SEARCH)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_ALTERNATIVE)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_APP_BROWSER)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_APP_CALCULATOR)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_APP_CALENDAR)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_APP_CONTACTS)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_APP_EMAIL)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_APP_GALLERY)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_APP_MAPS)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_APP_MARKET)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_APP_MESSAGING)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_APP_MUSIC)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_BROWSABLE)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_CAR_DOCK)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_CAR_MODE)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_DEFAULT)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_DESK_DOCK)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_DEVELOPMENT_PREFERENCE)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_EMBED)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_FRAMEWORK_INSTRUMENTATION_TEST)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_HE_DESK_DOCK)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_HOME)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_INFO)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_LAUNCHER)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_LEANBACK_LAUNCHER)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_LE_DESK_DOCK)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_MONKEY)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_OPENABLE)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_PREFERENCE)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_SAMPLE_CODE)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_SELECTED_ALTERNATIVE)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_TAB)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_TEST)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_TYPED_OPENABLE)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_UNIT_TEST)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_VOICE)
		AMTRS_JAVA_DEFINE_FIELD(String, CATEGORY_VR_HOME)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_ALARM_COUNT)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_ALLOW_MULTIPLE)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_ALLOW_REPLACE)


		AMTRS_JAVA_DEFINE_INIT(
			  void(Intent)
			, void(String)
			, void(String, Uri)
			, void(Context, Class)
			, void(String, Uri, Context, Class)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(addCategory
			, Intent(String category)
		)

		AMTRS_JAVA_DEFINE_METHOD(getData
			, Uri()
		)

		AMTRS_JAVA_DEFINE_METHOD(getIntExtra
			, jint(String, jint)
		)

		AMTRS_JAVA_DEFINE_METHOD(getStringExtra
			, String(String)
		)

		AMTRS_JAVA_DEFINE_METHOD(putExtra
			, Intent(String, jboolean)
			, Intent(String, jbyte)
			, Intent(String, jchar)
			, Intent(String, jshort)
			, Intent(String, jint)
			, Intent(String, jlong)
			, Intent(String, jfloat)
			, Intent(String, jdouble)
			, Intent(String, String)
//			, Intent(String, CharSequence)
//			, Intent(String, Parcelable)
//			, Intent(String, Parcelable[])
//			, Intent(String, Serializable)
//			, Intent(String, jboolean[])
//			, Intent(String, jbyte[])
//			, Intent(String, jchar[])
//			, Intent(String, jshort[])
//			, Intent(String, jint[])
//			, Intent(String, jlong[])
//			, Intent(String, jfloat[])
//			, Intent(String, jdouble[])
//			, Intent(String, String[])
//			, Intent(String, CharSequence[])
//			, Intent(String, Bundle)
		)

		AMTRS_JAVA_DEFINE_METHOD(putExtras
			, Intent(Intent)
			, Intent(Bundle)
		)

		AMTRS_JAVA_DEFINE_METHOD(setType
			, Intent(String type)
		)

	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
