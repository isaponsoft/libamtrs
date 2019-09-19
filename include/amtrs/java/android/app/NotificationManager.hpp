/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_app_NotificationManager__hpp
#define	__libamtrs__android__java_classes__android_app_NotificationManager__hpp
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::contet {
struct	ComponentName;
}
namespace android::app {

struct	AutomaticZenRule;
struct	NotificationChannel;
struct	NotificationChannelGroup;

AMTRS_JAVA_DEFINE_CLASS(NotificationManager, java::lang::Object)
{
	using	AutomaticZenRule			= android::app::AutomaticZenRule;
	using	NotificationChannel			= android::app::NotificationChannel;
	using	NotificationChannelGroup	= android::app::NotificationChannelGroup;
	using	ComponentName				= android::contet::ComponentName;
	using	String						= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/app/NotificationManager");

	struct	Policy;


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(String,		ACTION_APP_BLOCK_STATE_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String,		ACTION_INTERRUPTION_FILTER_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String,		ACTION_NOTIFICATION_CHANNEL_BLOCK_STATE_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String,		ACTION_NOTIFICATION_CHANNEL_GROUP_BLOCK_STATE_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String,		ACTION_NOTIFICATION_POLICY_ACCESS_GRANTED_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String,		ACTION_NOTIFICATION_POLICY_CHANGED)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_BLOCKED_STATE)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_NOTIFICATION_CHANNEL_GROUP_ID)
		AMTRS_JAVA_DEFINE_FIELD(String,		EXTRA_NOTIFICATION_CHANNEL_ID)
		AMTRS_JAVA_DEFINE_FIELD(jint,		IMPORTANCE_DEFAULT)
		AMTRS_JAVA_DEFINE_FIELD(jint,		IMPORTANCE_HIGH)
		AMTRS_JAVA_DEFINE_FIELD(jint,		IMPORTANCE_LOW)
		AMTRS_JAVA_DEFINE_FIELD(jint,		IMPORTANCE_MAX)
		AMTRS_JAVA_DEFINE_FIELD(jint,		IMPORTANCE_MIN)
		AMTRS_JAVA_DEFINE_FIELD(jint,		IMPORTANCE_NONE)
		AMTRS_JAVA_DEFINE_FIELD(jint,		IMPORTANCE_UNSPECIFIED)
		AMTRS_JAVA_DEFINE_FIELD(jint,		INTERRUPTION_FILTER_ALARMS)
		AMTRS_JAVA_DEFINE_FIELD(jint,		INTERRUPTION_FILTER_ALL)
		AMTRS_JAVA_DEFINE_FIELD(jint,		INTERRUPTION_FILTER_NONE)
		AMTRS_JAVA_DEFINE_FIELD(jint,		INTERRUPTION_FILTER_PRIORITY)
		AMTRS_JAVA_DEFINE_FIELD(jint,		INTERRUPTION_FILTER_UNKNOWN)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(addAutomaticZenRule
			, String(AutomaticZenRule automaticZenRule)
		)

		AMTRS_JAVA_DEFINE_METHOD(areNotificationsEnabled
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(cancel
			, void(jint id)
			, void(String tag, jint id)
		)

		AMTRS_JAVA_DEFINE_METHOD(cancelAll
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(createNotificationChannel
			, void(NotificationChannel channel)
		)

		AMTRS_JAVA_DEFINE_METHOD(createNotificationChannelGroup
			, void(NotificationChannelGroup group)
		)
/*
		AMTRS_JAVA_DEFINE_METHOD(createNotificationChannelGroups
			, void(NotificationChannelGroup group)
		)

		AMTRS_JAVA_DEFINE_METHOD(createNotificationChannels
			, void(NotificationChannelGroup group)
		)
*/

		AMTRS_JAVA_DEFINE_METHOD(deleteNotificationChannel
			, void(String channelId)
		)

		AMTRS_JAVA_DEFINE_METHOD(deleteNotificationChannelGroup
			, void(String groupId)
		)

		AMTRS_JAVA_DEFINE_METHOD(getAutomaticZenRule
			, AutomaticZenRule(String id)
		)

		AMTRS_JAVA_DEFINE_METHOD(getCurrentInterruptionFilter
			, jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(getImportance
			, jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(getNotificationChannel
			, NotificationChannel(String channelId)
		)

		AMTRS_JAVA_DEFINE_METHOD(getNotificationChannelGroup
			, NotificationChannelGroup(String channelGroupId)
		)

		AMTRS_JAVA_DEFINE_METHOD(isNotificationListenerAccessGranted
			, jboolean(ComponentName listener)
		)

		AMTRS_JAVA_DEFINE_METHOD(isNotificationPolicyAccessGranted
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(notify
			, void(jint id, Notification notification)
			, void(String tag, int id, Notification notification)
		)

		AMTRS_JAVA_DEFINE_METHOD(removeAutomaticZenRule
			, jboolean(String id)
		)

		AMTRS_JAVA_DEFINE_METHOD(setInterruptionFilter
			, void(jint interruptionFilter)
		)

		AMTRS_JAVA_DEFINE_METHOD(setNotificationPolicy
			, void(Policy policy)
		)

		AMTRS_JAVA_DEFINE_METHOD(updateAutomaticZenRule
			, jboolean(String id, AutomaticZenRule automaticZenRule)
		)

	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
