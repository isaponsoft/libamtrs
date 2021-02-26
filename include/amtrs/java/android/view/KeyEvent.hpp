/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_view_KeyEvent__hpp
#define	__libamtrs__android__java_classes__android_view_KeyEvent__hpp
#include <amtrs/java/android/view/InputEvent.hpp>
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::view {

// https://developer.android.com/reference/android/view/KeyEvent
AMTRS_JAVA_DEFINE_CLASS(KeyEvent, InputEvent)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/view/KeyEvent");


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

		AMTRS_JAVA_DEFINE_METHOD(getDeviceId
			, jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(getDownTime
			, jlong()
		)

		AMTRS_JAVA_DEFINE_METHOD(getEventTime
			, jlong()
		)

		AMTRS_JAVA_DEFINE_METHOD(getFlags
			, jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(getKeyCode
			, jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(getRepeatCount
			, jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(isAltPressed
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(isCanceled
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(isCapsLockOn
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(isCtrlPressed
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(isFunctionPressed
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(isGamepadButton
			, jboolean(jint keyCode)
		)

		AMTRS_JAVA_DEFINE_METHOD(isLongPress
			, jboolean()
		)
 

		AMTRS_JAVA_DEFINE_METHOD(isShiftPressed
			, jboolean()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
