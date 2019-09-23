/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_view_MotionEvent__hpp
#define	__libamtrs__android__java_classes__android_view_MotionEvent__hpp
#include <amtrs/java/android/view/InputEvent.hpp>
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::view {

// https://developer.android.com/reference/android/app/Activity
AMTRS_JAVA_DEFINE_CLASS(MotionEvent, InputEvent)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/view/MotionEvent");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(getAction
			, jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(getPointerCount
			, jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(getPointerId
			, jint(int pointerIndex)
		)

		AMTRS_JAVA_DEFINE_METHOD(getX
			, jfloat(int pointerIndex)
			, jfloat()
		)

		AMTRS_JAVA_DEFINE_METHOD(getY
			, jfloat(int pointerIndex)
			, jfloat()
		)

	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
