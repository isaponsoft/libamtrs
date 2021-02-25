/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_widget__RelativeLayout__hpp
#define	__libamtrs__android__java_classes__android_widget__RelativeLayout__hpp
#include "../view/ViewGroup.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::content {
struct	Context;
}
namespace android::util {
struct	AttributeSet;
}
namespace android::widget {

// https://developer.android.com/reference/android/widget/TimePicker
AMTRS_JAVA_DEFINE_CLASS(RelativeLayout, android::view::ViewGroup)
{

	using	Context			= android::content::Context;
	using	AttributeSet	= android::util::AttributeSet;


	AMTRS_JAVA_CLASS_SIGNATURE("android/widget/RelativeLayout");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(jint	, ABOVE)
		AMTRS_JAVA_DEFINE_FIELD(jint	, ALIGN_BASELINE)
		AMTRS_JAVA_DEFINE_FIELD(jint	, ALIGN_BOTTOM)
		AMTRS_JAVA_DEFINE_FIELD(jint	, ALIGN_END)
		AMTRS_JAVA_DEFINE_FIELD(jint	, ALIGN_LEFT)
		AMTRS_JAVA_DEFINE_FIELD(jint	, ALIGN_PARENT_BOTTOM)
		AMTRS_JAVA_DEFINE_FIELD(jint	, ALIGN_PARENT_END)
		AMTRS_JAVA_DEFINE_FIELD(jint	, ALIGN_PARENT_LEFT)
		AMTRS_JAVA_DEFINE_FIELD(jint	, ALIGN_PARENT_RIGHT)
		AMTRS_JAVA_DEFINE_FIELD(jint	, ALIGN_PARENT_START)
		AMTRS_JAVA_DEFINE_FIELD(jint	, ALIGN_PARENT_TOP)
		AMTRS_JAVA_DEFINE_FIELD(jint	, ALIGN_RIGHT)
		AMTRS_JAVA_DEFINE_FIELD(jint	, ALIGN_START)
		AMTRS_JAVA_DEFINE_FIELD(jint	, ALIGN_TOP)
		AMTRS_JAVA_DEFINE_FIELD(jint	, BELOW)
		AMTRS_JAVA_DEFINE_FIELD(jint	, CENTER_HORIZONTAL)
		AMTRS_JAVA_DEFINE_FIELD(jint	, CENTER_IN_PARENT)
		AMTRS_JAVA_DEFINE_FIELD(jint	, CENTER_VERTICAL)
		AMTRS_JAVA_DEFINE_FIELD(jint	, END_OF)
		AMTRS_JAVA_DEFINE_FIELD(jint	, LEFT_OF)
		AMTRS_JAVA_DEFINE_FIELD(jint	, RIGHT_OF)
		AMTRS_JAVA_DEFINE_FIELD(jint	, START_OF)
		AMTRS_JAVA_DEFINE_FIELD(jint	, TRUE)
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


	AMTRS_JAVA_DEFINE_CLASS(LayoutParams, android::view::ViewGroup::MarginLayoutParams)
	{

		AMTRS_JAVA_CLASS_SIGNATURE("android/widget/RelativeLayout$LayoutParams");


		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;

			AMTRS_JAVA_DEFINE_INIT(
				  void(Context c, AttributeSet attrs)
				, void(jint w, jint h)
				, void(ViewGroup::LayoutParams source)
				, void(ViewGroup::MarginLayoutParams source)
				, void(LayoutParams source)
			)
		};


		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;

			AMTRS_JAVA_DEFINE_METHOD(addRule
				, void(jint verb, jint subject)
				, void(jint verb)
			)
		};


	};


};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
