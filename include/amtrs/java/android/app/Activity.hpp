/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_app_Activity__hpp
#define	__libamtrs__android__java_classes__android_app_Activity__hpp
#include <amtrs/java/android/view/ContextThemeWrapper.hpp>
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::view {
struct	Window;
struct	WindowManager;
}

namespace android::app {

// https://developer.android.com/reference/android/app/Activity
AMTRS_JAVA_DEFINE_CLASS(Activity, android::view::ContextThemeWrapper)
{
	using	Intent			= android::content::Intent;
	using	Bundle			= android::os::Bundle;
	using	Window			= android::view::Window;
	using	WindowManager	= android::view::WindowManager;


	AMTRS_JAVA_CLASS_SIGNATURE("android/app/Activity");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(finish
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(getWindow
			, Window()
		)

		AMTRS_JAVA_DEFINE_METHOD(getWindowManager 
			, WindowManager()
		)

		AMTRS_JAVA_DEFINE_METHOD(isFinishing
			, void()
		)


		AMTRS_JAVA_DEFINE_METHOD(onActivityResult 
			, void(int requestCode, int resultCode, Intent data)
		)


		AMTRS_JAVA_DEFINE_METHOD(startActivityForResult
			, void(Intent intent, int requestCode)
			, void(Intent intent, int requestCode, Bundle options)
		)

		AMTRS_JAVA_DEFINE_METHOD(onPause
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(onStop
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(onStart
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(onResume
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(setVolumeControlStream
			, void(int streamType)
		)

	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
