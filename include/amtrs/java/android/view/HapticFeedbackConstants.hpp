/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_view_HapticFeedbackConstants__hpp
#define	__libamtrs__android__java_classes__android_view_HapticFeedbackConstants__hpp
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::view {

// https://developer.android.com/reference/android/app/Activity
AMTRS_JAVA_DEFINE_CLASS(HapticFeedbackConstants, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/view/HapticFeedbackConstants");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(int, CLOCK_TICK)
		AMTRS_JAVA_DEFINE_FIELD(int, CONTEXT_CLICK)
		AMTRS_JAVA_DEFINE_FIELD(int, FLAG_IGNORE_GLOBAL_SETTING)
		AMTRS_JAVA_DEFINE_FIELD(int, FLAG_IGNORE_VIEW_SETTING)
		AMTRS_JAVA_DEFINE_FIELD(int, KEYBOARD_PRESS)
		AMTRS_JAVA_DEFINE_FIELD(int, KEYBOARD_RELEASE)
		AMTRS_JAVA_DEFINE_FIELD(int, KEYBOARD_TAP)
		AMTRS_JAVA_DEFINE_FIELD(int, LONG_PRESS)
		AMTRS_JAVA_DEFINE_FIELD(int, TEXT_HANDLE_MOVE)
		AMTRS_JAVA_DEFINE_FIELD(int, VIRTUAL_KEY)
		AMTRS_JAVA_DEFINE_FIELD(int, VIRTUAL_KEY_RELEASE)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(performHapticFeedback
			, void(int feedbackConstant)
			, void(int feedbackConstant, int flag)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
