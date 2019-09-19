﻿/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_view_View__hpp
#define	__libamtrs__android__java_classes__android_view_View__hpp
#include <amtrs/java/android/content/ContextWrapper.hpp>
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::view {

// https://developer.android.com/reference/android/app/Activity
AMTRS_JAVA_DEFINE_CLASS(View, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/view/View");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(performHapticFeedback
			, jboolean(int feedbackConstant)
			, jboolean(int feedbackConstant, int flag)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif