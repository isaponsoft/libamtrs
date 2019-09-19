/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_content_Context__hpp
#define	__libamtrs__android__java_classes__android_content_Context__hpp
#include <amtrs/java/def.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::content::pm {
	struct	ApplicationInfo;
	struct	PackageItemInfo;
	struct	PackageManager;
}

namespace android::content::res {
	struct	AssetManager;
	struct	Configuration;
	struct	ColorStateList;
	struct	Resources;
	struct	TypedArray;
}

namespace android::database::sqlite {
	struct	SQLiteClosable ;
}

namespace android::graphics {
	struct	Bitmap;

	namespace drawable {
		struct	Drawable;
	}
}

namespace android::net {
	struct	Uri;
}

namespace android::os {
	struct	Bundle;
	struct	Looper;
	struct	Handler;
	struct	Parcelable;
	struct	UserHandle;
}

namespace android::util {
	struct	AttributeSet ;
}

namespace android::view {
	struct	Display;
}

namespace java::io {
	struct	File;
	struct	FileInputStream;
	struct	FileOutputStream;
	struct	InputStream;
	struct	OutputStream;
}


namespace android::content {

struct	BroadcastReceiver;
struct	ComponentCallbacks;
struct	ComponentName;
struct	ContentResolver;
struct	Intent;
struct	IntentFilter;
struct	IntentSender;
struct	ServiceConnection;
struct	SharedPreferences;

// https://developer.android.com/reference/android/content/Context.html
AMTRS_JAVA_DEFINE_CLASS(Context , java::lang::Object)
{
	using	ApplicationInfo		= android::content::pm::ApplicationInfo;
	using	PackageItemInfo		= android::content::pm::PackageItemInfo;
	using	PackageManager		= android::content::pm::PackageManager;
	using	AssetManager		= android::content::res::AssetManager;
	using	ColorStateList		= android::content::res::ColorStateList;
	using	Configuration		= android::content::res::Configuration;
	using	Resources			= android::content::res::Resources;
	using	TypedArray			= android::content::res::TypedArray;
	using	SQLiteClosable		= android::database::sqlite::SQLiteClosable;
	using	Bitmap				= android::graphics::Bitmap;
	using	Drawable			= android::graphics::drawable::Drawable;
	using	Uri					= android::net::Uri;
	using	Bundle				= android::os::Bundle;
	using	Handler				= android::os::Handler;
	using	Looper				= android::os::Looper;
	using	Parcelable			= android::os::Parcelable;
	using	UserHandle			= android::os::UserHandle;
	using	AttributeSet		= android::util::AttributeSet;
	using	Display				= android::view::Display;
	using	File				= java::io::File;
	using	FileInputStream		= java::io::FileInputStream;
	using	FileOutputStream	= java::io::FileOutputStream;
	using	InputStream			= java::io::InputStream;
	using	OutputStream		= java::io::OutputStream;
	using	CharSequence		= java::lang::CharSequence;
	using	ClassLoader			= java::lang::ClassLoader;
	using	Class				= java::lang::Class;
	using	String				= java::lang::String;
	using	Object				= java::lang::Object;

	AMTRS_JAVA_CLASS_SIGNATURE("android/content/Context");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(String, ACCESSIBILITY_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACCOUNT_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTIVITY_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, ALARM_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, APPWIDGET_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, APP_OPS_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, AUDIO_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(jint, BIND_ABOVE_CLIENT)
		AMTRS_JAVA_DEFINE_FIELD(jint, BIND_ADJUST_WITH_ACTIVITY)
		AMTRS_JAVA_DEFINE_FIELD(jint, BIND_ALLOW_OOM_MANAGEMENT)
		AMTRS_JAVA_DEFINE_FIELD(jint, BIND_AUTO_CREATE)
		AMTRS_JAVA_DEFINE_FIELD(jint, BIND_DEBUG_UNBIND)
		AMTRS_JAVA_DEFINE_FIELD(jint, BIND_EXTERNAL_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(jint, BIND_IMPORTANT)
		AMTRS_JAVA_DEFINE_FIELD(jint, BIND_NOT_FOREGROUND)
		AMTRS_JAVA_DEFINE_FIELD(jint, BIND_WAIVE_PRIORITY)
		AMTRS_JAVA_DEFINE_FIELD(String, BLUETOOTH_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, CAMERA_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, CAPTIONING_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, CARRIER_CONFIG_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, CLIPBOARD_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, COMPANION_DEVICE_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, CONNECTIVITY_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, CONSUMER_IR_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(jint, CONTEXT_IGNORE_SECURITY)
		AMTRS_JAVA_DEFINE_FIELD(jint, CONTEXT_INCLUDE_CODE)
		AMTRS_JAVA_DEFINE_FIELD(jint, CONTEXT_RESTRICTED)
		AMTRS_JAVA_DEFINE_FIELD(String, DEVICE_POLICY_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, DISPLAY_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, DOWNLOAD_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, DROPBOX_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, FINGERPRINT_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, HARDWARE_PROPERTIES_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, INPUT_METHOD_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, JOB_SCHEDULER_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, KEYGUARD_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, LAUNCHER_APPS_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, LAYOUT_INFLATER_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, LOCATION_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, MEDIA_PROJECTION_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, MEDIA_ROUTER_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, MEDIA_SESSION_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, MIDI_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(jint, MODE_APPEND)
		AMTRS_JAVA_DEFINE_FIELD(jint, MODE_ENABLE_WRITE_AHEAD_LOGGING)
		AMTRS_JAVA_DEFINE_FIELD(jint, MODE_MULTI_PROCESS)
		AMTRS_JAVA_DEFINE_FIELD(jint, MODE_NO_LOCALIZED_COLLATORS)
		AMTRS_JAVA_DEFINE_FIELD(jint, MODE_PRIVATE)
		AMTRS_JAVA_DEFINE_FIELD(jint, MODE_WORLD_READABLE)
		AMTRS_JAVA_DEFINE_FIELD(jint, MODE_WORLD_WRITEABLE)
		AMTRS_JAVA_DEFINE_FIELD(String, NETWORK_STATS_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, NFC_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, NOTIFICATION_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, NSD_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, POWER_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, PRINT_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(jint, RECEIVER_VISIBLE_TO_INSTANT_APPS)
		AMTRS_JAVA_DEFINE_FIELD(String, RESTRICTIONS_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, SEARCH_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, SENSOR_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, SHORTCUT_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, STORAGE_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, STORAGE_STATS_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, SYSTEM_HEALTH_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, TELECOM_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, TELEPHONY_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, TELEPHONY_SUBSCRIPTION_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, TEXT_CLASSIFICATION_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, TEXT_SERVICES_MANAGER_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, TV_INPUT_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, UI_MODE_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, USAGE_STATS_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, USB_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, USER_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, VIBRATOR_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, WALLPAPER_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, WIFI_AWARE_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, WIFI_P2P_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, WIFI_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, WINDOW_SERVICE)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;



		AMTRS_JAVA_DEFINE_METHOD(bindService
			, jboolean(Intent service, ServiceConnection conn, int flags)
		)

		AMTRS_JAVA_DEFINE_METHOD(checkCallingOrSelfPermission
			, jint(String permission)
		)

		AMTRS_JAVA_DEFINE_METHOD(checkCallingOrSelfUriPermission
			, jint(Uri uri, int modeFlags)
		)

		AMTRS_JAVA_DEFINE_METHOD(checkCallingPermission
			, jint(String permission)
		)

		AMTRS_JAVA_DEFINE_METHOD(checkCallingUriPermission
			, jint(Uri uri, int modeFlags)
		)

		AMTRS_JAVA_DEFINE_METHOD(checkPermission
			, jint(String permission, int pid, int uid)
			, jint(String permission)
		)

		AMTRS_JAVA_DEFINE_METHOD(checkUriPermission
			, jint(Uri uri, String readPermission, String writePermission, int pid, int uid, int modeFlags)
			, jint(Uri uri, int pid, int uid, int modeFlags)
		)

		AMTRS_JAVA_DEFINE_METHOD(clearWallpaper
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(createConfigurationContext
			, Context(Configuration overrideConfiguration)
		)

		AMTRS_JAVA_DEFINE_METHOD(createContextForSplit
			, Context(String splitName)
		)

		AMTRS_JAVA_DEFINE_METHOD(createDeviceProtectedStorageContext
			, Context()
		)

		AMTRS_JAVA_DEFINE_METHOD(createDisplayContext
			, Context(Display display)
		)

		AMTRS_JAVA_DEFINE_METHOD(createPackageContext
			, Context(String packageName, int flags)
		)
/*
		AMTRS_JAVA_DEFINE_METHOD(databaseList
			, String[]()
		)
*/

		AMTRS_JAVA_DEFINE_METHOD(deleteDatabase
			, jboolean(String name)
		)

		AMTRS_JAVA_DEFINE_METHOD(deleteFile
			, jboolean(String name)
		)

		AMTRS_JAVA_DEFINE_METHOD(deleteSharedPreferences
			, jboolean(String name)
		)

		AMTRS_JAVA_DEFINE_METHOD(enforceCallingOrSelfPermission
			, void(String permission, String message)
		)

		AMTRS_JAVA_DEFINE_METHOD(enforceCallingOrSelfUriPermission
			, void(Uri uri, int modeFlags, String message)
		)

		AMTRS_JAVA_DEFINE_METHOD(enforceCallingPermission
			, void(String permission, String message)
		)

		AMTRS_JAVA_DEFINE_METHOD(enforceCallingUriPermission
			, void(Uri uri, int modeFlags, String message)
		)

		AMTRS_JAVA_DEFINE_METHOD(enforcePermission
			, void(String permission, int pid, int uid, String message)
		)

		AMTRS_JAVA_DEFINE_METHOD(enforceUriPermission
			, void(Uri uri, String readPermission, String writePermission, int pid, int uid, int modeFlags, String message)
			, void(Uri uri, int pid, int uid, int modeFlags, String message)
		)

/*		AMTRS_JAVA_DEFINE_METHOD(fileList
			, String[]()
		)*/

		AMTRS_JAVA_DEFINE_METHOD(getApplicationContext
			, Context()
		)

		AMTRS_JAVA_DEFINE_METHOD(getApplicationInfo
			, ApplicationInfo()
		)

		AMTRS_JAVA_DEFINE_METHOD(getAssets
			, AssetManager()
		)

		AMTRS_JAVA_DEFINE_METHOD(getCacheDir
			, File()
		)

		AMTRS_JAVA_DEFINE_METHOD(getClassLoader
			, ClassLoader()
		)

		AMTRS_JAVA_DEFINE_METHOD(getCodeCacheDir
			, File()
		)

		AMTRS_JAVA_DEFINE_METHOD(getColor
			, int(int id)
		)

		AMTRS_JAVA_DEFINE_METHOD(getColorStateList
			, ColorStateList(int id)
		)

		AMTRS_JAVA_DEFINE_METHOD(getContentResolver
			, ContentResolver()
		)

		AMTRS_JAVA_DEFINE_METHOD(getDataDir
			, File()
		)

		AMTRS_JAVA_DEFINE_METHOD(getDatabasePath
			, File(String name)
		)

		AMTRS_JAVA_DEFINE_METHOD(getDir
			, File(String name, int mode)
		)

		AMTRS_JAVA_DEFINE_METHOD(getDrawable
			, Drawable(int id)
		)

		AMTRS_JAVA_DEFINE_METHOD(getExternalCacheDir
			, File()
		)

/*		AMTRS_JAVA_DEFINE_METHOD(getExternalCacheDirs
			, File[]()
		)*/

		AMTRS_JAVA_DEFINE_METHOD(getExternalFilesDir
			, File(String type)
		)

/*		AMTRS_JAVA_DEFINE_METHOD(getExternalFilesDirs
			, File[](String type)
		)*/

		AMTRS_JAVA_DEFINE_METHOD(getExternalMediaDirs
			, File()
		)

		AMTRS_JAVA_DEFINE_METHOD(getFileStreamPath
			, File(String name)
		)

		AMTRS_JAVA_DEFINE_METHOD(getFilesDir
			, File()
		)

		AMTRS_JAVA_DEFINE_METHOD(getMainLooper
			, Looper()
		)

		AMTRS_JAVA_DEFINE_METHOD(getNoBackupFilesDir
			, File()
		)

		AMTRS_JAVA_DEFINE_METHOD(getObbDir
			, File()
		)

/*		AMTRS_JAVA_DEFINE_METHOD(getObbDirs
			, File[]()
		)*/

		AMTRS_JAVA_DEFINE_METHOD(getPackageCodePath
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getPackageManager
			, PackageManager()
		)

		AMTRS_JAVA_DEFINE_METHOD(getPackageName
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getPackageResourcePath
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getResources
			, Resources()
		)

		AMTRS_JAVA_DEFINE_METHOD(getSharedPreferences
			, SharedPreferences(String name, int mode)
			, String(int resId)
		)
		//, String(int resId, Object... formatArgs)

		AMTRS_JAVA_DEFINE_METHOD(getSystemService
			, Object(Class serviceClass)
			, Object(String name)
		)

		AMTRS_JAVA_DEFINE_METHOD(getText
			, CharSequence(int resId)
		)

/*		AMTRS_JAVA_DEFINE_METHOD(getTheme
			, Resource::Theme(int resId)
		)*/

		AMTRS_JAVA_DEFINE_METHOD(getWallpaper
			, Drawable()
		)

		AMTRS_JAVA_DEFINE_METHOD(getWallpaperDesiredMinimumHeight
			, int()
		)

		AMTRS_JAVA_DEFINE_METHOD(getWallpaperDesiredMinimumWidth
			, int()
		)

		AMTRS_JAVA_DEFINE_METHOD(grantUriPermission
			, void(String toPackage, Uri uri, int modeFlags)
		)

		AMTRS_JAVA_DEFINE_METHOD(isDeviceProtectedStorage
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(isRestricted
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(moveDatabaseFrom
			, jboolean(Context sourceContext, String name)
		)

		AMTRS_JAVA_DEFINE_METHOD(moveSharedPreferencesFrom
			, jboolean(Context sourceContext, String name)
		)

		AMTRS_JAVA_DEFINE_METHOD(obtainStyledAttributes
			, TypedArray(AttributeSet set, int attrs[])
			, TypedArray(AttributeSet set, int attrs[], int defStyleAttr, int defStyleRes)
			, TypedArray(int resid, int attrs[])
			, TypedArray(int attrs[])
		)

		AMTRS_JAVA_DEFINE_METHOD(openFileInput
			, FileInputStream(String name)
		)

		AMTRS_JAVA_DEFINE_METHOD(openFileOutput
			, FileOutputStream(String name, int mode)
		)

/*		AMTRS_JAVA_DEFINE_METHOD(openOrCreateDatabase
			, SQLiteClosable(String name, int mode, SQLiteDatabase.CursorFactory factory, DatabaseErrorHandler errorHandler)
			, SQLiteClosable(String name, int mode, SQLiteDatabase.CursorFactory factory)
		)*/

		AMTRS_JAVA_DEFINE_METHOD(peekWallpaper
			, Drawable()
		)

		AMTRS_JAVA_DEFINE_METHOD(registerComponentCallbacks
			, void(ComponentCallbacks callback)
		)

		AMTRS_JAVA_DEFINE_METHOD(registerReceiver
			, Intent(BroadcastReceiver receiver, IntentFilter filter)
			, Intent(BroadcastReceiver receiver, IntentFilter filter, int flags)
			, Intent(BroadcastReceiver receiver, IntentFilter filter, String broadcastPermission, Handler scheduler, int flags)
			, Intent(BroadcastReceiver receiver, IntentFilter filter, String broadcastPermission, Handler scheduler)
		)

		AMTRS_JAVA_DEFINE_METHOD(removeStickyBroadcast
			, void(Intent intent)
		)

		AMTRS_JAVA_DEFINE_METHOD(removeStickyBroadcastAsUser
			, void(Intent intent, UserHandle user)
		)

		AMTRS_JAVA_DEFINE_METHOD(revokeUriPermission
			, void(Uri uri, int modeFlags)
			, void(String toPackage, Uri uri, int modeFlags)
		)

		AMTRS_JAVA_DEFINE_METHOD(sendBroadcast
			, void(Intent intent, String receiverPermission)
			, void(Intent intent)
		)

		AMTRS_JAVA_DEFINE_METHOD(sendBroadcastAsUser
			, void(Intent intent, UserHandle user)
			, void(Intent intent, UserHandle user, String receiverPermission)
		)

		AMTRS_JAVA_DEFINE_METHOD(sendOrderedBroadcast
			, void(Intent intent, String receiverPermission, BroadcastReceiver resultReceiver, Handler scheduler, int initialCode, String initialData, Bundle initialExtras)
			, void(Intent intent, String receiverPermission)
		)

		AMTRS_JAVA_DEFINE_METHOD(sendOrderedBroadcastAsUser
			, void(Intent intent, UserHandle user, String receiverPermission, BroadcastReceiver resultReceiver, Handler scheduler, int initialCode, String initialData, Bundle initialExtras)
		)

		AMTRS_JAVA_DEFINE_METHOD(sendStickyBroadcast
			, void(Intent intent)
		)

		AMTRS_JAVA_DEFINE_METHOD(sendStickyBroadcastAsUser
			, void(Intent intent, UserHandle user)
		)

		AMTRS_JAVA_DEFINE_METHOD(sendStickyOrderedBroadcast
			, void(Intent intent, BroadcastReceiver resultReceiver, Handler scheduler, int initialCode, String initialData, Bundle initialExtras)
		)

		AMTRS_JAVA_DEFINE_METHOD(sendStickyOrderedBroadcastAsUser
			, void(Intent intent, BroadcastReceiver resultReceiver, Handler scheduler, int initialCode, String initialData, Bundle initialExtras)
		)

		AMTRS_JAVA_DEFINE_METHOD(setTheme
			, void(int resid)
		)

		AMTRS_JAVA_DEFINE_METHOD(setWallpaper
			, void(Bitmap bitmap)
			, void(InputStream data)
		)

		AMTRS_JAVA_DEFINE_METHOD(startActivities
			, void(Intent intents[], Bundle options)
			, void(Intent intents[])
		)

		AMTRS_JAVA_DEFINE_METHOD(startActivity
			, void(Intent intent)
			, void(Intent intent, Bundle options)
		)

		AMTRS_JAVA_DEFINE_METHOD(startForegroundService
			, ComponentName(Intent intents[], Bundle options)
		)

		AMTRS_JAVA_DEFINE_METHOD(startInstrumentation
			, jboolean(ComponentName className, String profileFile, Bundle arguments)
		)

		AMTRS_JAVA_DEFINE_METHOD(startIntentSender
			, void(IntentSender intent, Intent fillInIntent, int flagsMask, int flagsValues, int extraFlags)
			, void(IntentSender intent, Intent fillInIntent, int flagsMask, int flagsValues, int extraFlags, Bundle options)
		)

		AMTRS_JAVA_DEFINE_METHOD(startService
			, ComponentName(Intent service)
		)

		AMTRS_JAVA_DEFINE_METHOD(stopService
			, jboolean(Intent service)
		)

		AMTRS_JAVA_DEFINE_METHOD(unbindService
			, void(ServiceConnection conn)
		)

		AMTRS_JAVA_DEFINE_METHOD(unregisterComponentCallbacks
			, void(ComponentCallbacks callback)
		)

		AMTRS_JAVA_DEFINE_METHOD(unregisterReceiver
			, void(BroadcastReceiver receiver)
		)

	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
