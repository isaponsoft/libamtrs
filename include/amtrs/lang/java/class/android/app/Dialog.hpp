/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_app_Dialog__hpp
#define	__libamtrs__android__java_classes__android_app_Dialog__hpp
#include <amtrs/java/android/view/ContextThemeWrapper.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::app {

// https://developer.android.com/reference/android/app/Dialog
AMTRS_JAVA_DEFINE_CLASS(Dialog, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/app/Dialog");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(cancel 
			, void()
		)


		AMTRS_JAVA_DEFINE_METHOD(closeOptionsMenu
			, void()
		)


		AMTRS_JAVA_DEFINE_METHOD(create
			, void()
		)


		AMTRS_JAVA_DEFINE_METHOD(dismiss
			, void()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
