/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__jp__libamtrs__window__DialogTimePicker__hpp
#define	__libamtrs__android__java_classes__jp__libamtrs__window__DialogTimePicker__hpp
#include <amtrs/java/android/support/v4/app/DialogFragment.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::content {
struct	Context;
}


namespace jp::libamtrs::window {

AMTRS_JAVA_DEFINE_CLASS(DialogTimePicker, android::support::v4::app::DialogFragment)
{
	using	Context		= android::content::Context;

	AMTRS_JAVA_CLASS_SIGNATURE("jp/libamtrs/window/DialogTimePicker");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(create
			, DialogTimePicker(jlong)
			, DialogTimePicker(jlong, jint _hourOfDay, jint _minuteOfHour)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(updateTime
			, void(jint _hourOfDay, jint _minuteOfHour)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
