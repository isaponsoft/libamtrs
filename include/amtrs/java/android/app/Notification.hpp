﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_app_Notification__hpp
#define	__libamtrs__android__java_classes__android_app_Notification__hpp
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::graphics::drawable {
struct	Icon;
}
namespace android::app {

struct	Parcel;


AMTRS_JAVA_DEFINE_CLASS(Notification, java::lang::Object)
{
	using	CharSequence	= java::lang::CharSequence;
	using	String			= java::lang::String;
	using	Parcel			= android::os::Parcel;
	using	Icon			= android::graphics::drawable::Icon;

	AMTRS_JAVA_CLASS_SIGNATURE("android/app/Notification");

	struct	Action;
	struct	BigPictureStyle;
	struct	BigTextStyle;
	struct	Builder;
	struct	CarExtender;
	struct	DecoratedCustomViewStyle;
	struct	DecoratedMediaCustomViewStyle;
	struct	Extender;
	struct	InboxStyle;
	struct	MediaStyle;
	struct	MessagingStyle;
	struct	Style;
	struct	WearableExtender;


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(jint,		BADGE_ICON_LARGE)
		AMTRS_JAVA_DEFINE_FIELD(jint,		BADGE_ICON_NONE)
		AMTRS_JAVA_DEFINE_FIELD(jint,		BADGE_ICON_SMALL)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_ALARM)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_CALL)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_EMAIL)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_ERROR)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_EVENT)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_MESSAGE)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_NAVIGATION)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_PROGRESS)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_PROMO)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_RECOMMENDATION)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_REMINDER)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_SOCIAL)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_STATUS)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_SYSTEM)
		AMTRS_JAVA_DEFINE_FIELD(String,		CATEGORY_TRANSPORT)
		AMTRS_JAVA_DEFINE_FIELD(jint,		COLOR_DEFAULT)
		AMTRS_JAVA_DEFINE_FIELD(jint,		DEFAULT_ALL)
		AMTRS_JAVA_DEFINE_FIELD(jint,		DEFAULT_LIGHTS)
		AMTRS_JAVA_DEFINE_FIELD(jint,		DEFAULT_SOUND)
		AMTRS_JAVA_DEFINE_FIELD(jint,		DEFAULT_VIBRATE)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_AUDIO_CONTENTS_URI)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_BACKGROUND_IMAGE_URI)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_BIG_TEXT)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_CHANNEL_GROUP_ID)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_CHANNEL_ID)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_CHRONOMETER_COUNT_DOWN)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_COLORIZED)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_COMPACT_ACTIONS)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_CONVERSATION_TITLE)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_HISTORIC_MESSAGES)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_INFO_TEXT)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_IS_GROUP_CONVERSATION)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_LARGE_ICON)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_LARGE_ICON_BIG)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_MEDIA_SESSION)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_MESSAGES)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_MESSAGING_PERSON)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_NOTIFICATION_ID)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_NOTIFICATION_TAG)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_PEOPLE)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_PEOPLE_LIST)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_PICTURE)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_PROGRESS)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_PROGRESS_INDETERMINATE)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_PROGRESS_MAX)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_REMOTE_INPUT_DRAFT)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_REMOTE_INPUT_HISTORY)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_SELF_DISPLAY_NAME)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_SHOW_CHRONOMETER)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_SHOW_WHEN)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_SMALL_ICON)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_SUB_TEXT)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_SUMMARY_TEXT)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_TEMPLATE)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_TEXT)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_TEXT_LINES)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_TITLE)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_TITLE_BIG)
		AMTRS_JAVA_DEFINE_FIELD(jint,		FLAG_AUTO_CANCEL)
		AMTRS_JAVA_DEFINE_FIELD(jint,		FLAG_FOREGROUND_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(jint,		FLAG_GROUP_SUMMARY)
		AMTRS_JAVA_DEFINE_FIELD(jint,		FLAG_HIGH_PRIORITY)
		AMTRS_JAVA_DEFINE_FIELD(jint,		FLAG_INSISTENT)
		AMTRS_JAVA_DEFINE_FIELD(jint,		FLAG_LOCAL_ONLY)
		AMTRS_JAVA_DEFINE_FIELD(jint,		FLAG_NO_CLEAR)
		AMTRS_JAVA_DEFINE_FIELD(jint,		FLAG_ONGOING_EVENT)
		AMTRS_JAVA_DEFINE_FIELD(jint,		FLAG_ONLY_ALERT_ONCE)
		AMTRS_JAVA_DEFINE_FIELD(jint,		FLAG_SHOW_LIGHTS)
		AMTRS_JAVA_DEFINE_FIELD(jint,		GROUP_ALERT_ALL)
		AMTRS_JAVA_DEFINE_FIELD(jint,		GROUP_ALERT_CHILDREN)
		AMTRS_JAVA_DEFINE_FIELD(jint,		GROUP_ALERT_SUMMARY)
		AMTRS_JAVA_DEFINE_FIELD(String,		INTENT_CATEGORY_NOTIFICATION_PREFERENCES)
		AMTRS_JAVA_DEFINE_FIELD(jint,		PRIORITY_DEFAULT)
		AMTRS_JAVA_DEFINE_FIELD(jint,		PRIORITY_HIGH)
		AMTRS_JAVA_DEFINE_FIELD(jint,		PRIORITY_LOW)
		AMTRS_JAVA_DEFINE_FIELD(jint,		PRIORITY_MAX)
		AMTRS_JAVA_DEFINE_FIELD(jint,		PRIORITY_MIN)
		AMTRS_JAVA_DEFINE_FIELD(jint,		STREAM_DEFAULT)
		AMTRS_JAVA_DEFINE_FIELD(jint,		VISIBILITY_PRIVATE)
		AMTRS_JAVA_DEFINE_FIELD(jint,		VISIBILITY_PUBLIC)
		AMTRS_JAVA_DEFINE_FIELD(jint,		VISIBILITY_SECRET)

		AMTRS_JAVA_DEFINE_INIT(
			  Notification()
			, Notification(jint icon, CharSequence tickerText, jlong when)
			, Notification(Parcel parcel)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		ANTRS_JAVA_DEFINE_METHOD(close
			, Notification()
		)

		ANTRS_JAVA_DEFINE_METHOD(describeContents
			, jint()
		)

		ANTRS_JAVA_DEFINE_METHOD(getBadgeIconType
			, jint()
		)

		ANTRS_JAVA_DEFINE_METHOD(getChannelId
			, String()
		)

		ANTRS_JAVA_DEFINE_METHOD(getGroup
			, String()
		)

		ANTRS_JAVA_DEFINE_METHOD(getGroupAlertBehavior
			, jint()
		)

		ANTRS_JAVA_DEFINE_METHOD(getLargeIcon
			, Icon()
		)

		ANTRS_JAVA_DEFINE_METHOD(getSettingsText
			, CharSequence()
		)

		ANTRS_JAVA_DEFINE_METHOD(getShortcutId
			, String()
		)

		ANTRS_JAVA_DEFINE_METHOD(getSmallIcon
			, Icon()
		)

		ANTRS_JAVA_DEFINE_METHOD(getSortKey
			, String()
		)

		ANTRS_JAVA_DEFINE_METHOD(getTimeoutAfter
			, jlong()
		)

		ANTRS_JAVA_DEFINE_METHOD(toString
			, String()
		)

		ANTRS_JAVA_DEFINE_METHOD(writeToParcel
			, void(Parcel parcel, jint flags)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
