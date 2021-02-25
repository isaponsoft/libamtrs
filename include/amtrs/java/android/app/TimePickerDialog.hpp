/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_app_TimePickerDialog__hpp
#define	__libamtrs__android__java_classes__android_app_TimePickerDialog__hpp
#include <amtrs/java/android/app/AlertDialog.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::os {
	struct	Bundle;
}

namespace android::widget {
	struct	TimePicker;
}

namespace android::app {

// https://developer.android.com/reference/android/app/AlertDialog
AMTRS_JAVA_DEFINE_CLASS(TimePickerDialog, AlertDialog)
{
	using	Bundle				= android::os::Bundle;

	AMTRS_JAVA_CLASS_SIGNATURE("android/app/TimePickerDialog");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(onSaveInstanceState
			, Bundle ()
		)

		AMTRS_JAVA_DEFINE_METHOD(show
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(updateTime
			, void(int hourOfDay, int minuteOfHour)
		)
	};



	AMTRS_JAVA_DEFINE_CLASS(OnTimeSetListener, classes::java::lang::Object)
	{
		using	TimePicker			= android::widget::TimePicker;

		AMTRS_JAVA_CLASS_SIGNATURE("android/app/TimePickerDialog$OnTimeSetListener");

		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;
		};

		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;

			AMTRS_JAVA_DEFINE_METHOD(onTimeSet
				, void(TimePicker view, int hourOfDay, int minute)
			)
		};
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
