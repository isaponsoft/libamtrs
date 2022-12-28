/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__jp__libamtrs__window__DialogTimePicker__hpp
#define	__libamtrs__android__java_classes__jp__libamtrs__window__DialogTimePicker__hpp
#include "../../../androidx/fragment/app/DialogFragment.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::content {
struct	Context;
}


namespace jp::libamtrs::window {

AMTRS_JAVA_DEFINE_CLASS(DialogTimePicker, androidx::fragment::app::DialogFragment)
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
