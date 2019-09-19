/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android__app__PendingIntent__hpp
#define	__libamtrs__android__java_classes__android__app__PendingIntent__hpp
#include <amtrs/java/android/app/AlertDialog.hpp>
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::content {
struct	Context;
struct	Intent;
struct	IntentSender;
}

namespace android::os {
struct	Bundle;
struct	Handler;
struct	Parcel;
struct	UserHandle;
}

namespace android::app {

AMTRS_JAVA_DEFINE_CLASS(PendingIntent, java::lang::Object) // java::implements<android.os.Parcelable>
{
	using	Context			= android::content::Context;
	using	Intent			= android::content::Intent;
	using	IntentSender	= android::content::IntentSender;
	using	Bundle			= android::os::Bundle;
	using	Handler			= android::os::Handler;
	using	Parcel			= android::os::Parcel;
	using	UserHandle		= android::os::UserHandle;
	using	String			= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/app/PendingIntent");


	AMTRS_JAVA_DEFINE_CLASS(CanceledException , java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/app/PendingIntent$CanceledException");

		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;
		};

		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;
		};
	};


	AMTRS_JAVA_DEFINE_CLASS(OnFinished , java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/app/PendingIntent$OnFinished");

		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;
		};

		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;
		};
	};


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getActivities
			, PendingIntent(Context context, jint requestCode, Intent intent, jint flags)
			, PendingIntent(Context context, jint requestCode, Intent intent, jint flags, Bundle options)
		)

		AMTRS_JAVA_DEFINE_METHOD(getBroadcast
			, PendingIntent(Context context, jint requestCode, Intent intent, jint flags)
		)

		AMTRS_JAVA_DEFINE_METHOD(getForegroundService
			, PendingIntent(Context context, jint requestCode, Intent intent, jint flags)
		)

		AMTRS_JAVA_DEFINE_METHOD(getService
			, PendingIntent(Context context, jint requestCode, Intent intent, jint flags)
		)

		AMTRS_JAVA_DEFINE_METHOD(readPendingIntentOrNullFromParcel
			, PendingIntent(Parcel in)
		)

		AMTRS_JAVA_DEFINE_METHOD(writePendingIntentOrNullToParcel
			, void(PendingIntent sender, Parcel out)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(jint,	FLAG_CANCEL_CURRENT)
		AMTRS_JAVA_DEFINE_FIELD(jint,	FLAG_IMMUTABLE)
		AMTRS_JAVA_DEFINE_FIELD(jint,	FLAG_NO_CREATE)
		AMTRS_JAVA_DEFINE_FIELD(jint,	FLAG_ONE_SHOT)
		AMTRS_JAVA_DEFINE_FIELD(jint,	FLAG_UPDATE_CURRENT)


		AMTRS_JAVA_DEFINE_METHOD(cancel
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(describeContents
			, jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(equals
			, jboolean(Object)
		)

		AMTRS_JAVA_DEFINE_METHOD(getCreatorPackage
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getCreatorUid
			, jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(getCreatorUserHandle
			, UserHandle()
		)

		AMTRS_JAVA_DEFINE_METHOD(getIntentSender
			, IntentSender()
		)

		AMTRS_JAVA_DEFINE_METHOD(getTargetPackage
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(hashCode
			, jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(send
			, void(Context context, jint code, Intent intent, OnFinished onFinished, Handler handler, String requiredPermission, Bundle options)
			, void(Context context, jint code, Intent intent, OnFinished onFinished, Handler handler, String requiredPermission)
			, void(Context context, jint code, Intent intent, OnFinished onFinished, Handler handler)
			, void(jint code, OnFinished onFinished, Handler handler)
			, void(Context context, jint code, Intent intent)
			, void(jint code)
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(toString
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(writeToParcel
			, void(Parcel out, int flags)
		)
	};

};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
