/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_view_Display__hpp
#define	__libamtrs__android__java_classes__android_view_Display__hpp
#include <amtrs/java/android/content/ContextWrapper.hpp>
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::view {

// https://developer.android.com/reference/android/view/Display
AMTRS_JAVA_DEFINE_CLASS(Display, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/view/Display");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getHeight
			, int()
		)

		AMTRS_JAVA_DEFINE_METHOD(getWidth
			, int()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
