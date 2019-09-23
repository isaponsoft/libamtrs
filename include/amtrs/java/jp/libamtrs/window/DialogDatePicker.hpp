/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__jp__libamtrs__window__DialogDatePicker__hpp
#define	__libamtrs__android__java_classes__jp__libamtrs__window__DialogDatePicker__hpp
#include <amtrs/java/android/support/v4/app/DialogFragment.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::content {
struct	Context;
}


namespace jp::libamtrs::window {

AMTRS_JAVA_DEFINE_CLASS(DialogDatePicker, android::support::v4::app::DialogFragment)
{
	using	Context		= android::content::Context;

	AMTRS_JAVA_CLASS_SIGNATURE("jp/libamtrs/window/DialogDatePicker");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(create
			, DialogDatePicker(jlong)
			, DialogDatePicker(jlong, jint _year, jint _mon, jint _day)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(updateDate
			, void(jint _year, jint _mon, jint _day)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
