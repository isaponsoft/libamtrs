/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_widget_TimePicker__hpp
#define	__libamtrs__android__java_classes__android_widget_TimePicker__hpp
#include <amtrs/java/android/widget/FrameLayout.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::widget {

// https://developer.android.com/reference/android/widget/TimePicker
AMTRS_JAVA_DEFINE_CLASS(TimePicker, FrameLayout)
{

	AMTRS_JAVA_CLASS_SIGNATURE("android/widget/TimePicker");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(getHour
			, int()
		)

		AMTRS_JAVA_DEFINE_METHOD(getMinute
			, int()
		)

		AMTRS_JAVA_DEFINE_METHOD(is24HourView
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(isEnabled
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(setHour
			, void(int hour)
		)

		AMTRS_JAVA_DEFINE_METHOD(setMinute
			, void(int minute)
		)

		AMTRS_JAVA_DEFINE_METHOD(validateInput
			, jboolean()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
