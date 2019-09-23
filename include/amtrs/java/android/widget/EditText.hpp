/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_widget_EditText__hpp
#define	__libamtrs__android__java_classes__android_widget_EditText__hpp
#include <amtrs/java/android/widget/TextView.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::widget {

// https://developer.android.com/reference/android/widget/EditText
AMTRS_JAVA_DEFINE_CLASS(EditText, TextView)
{

	AMTRS_JAVA_CLASS_SIGNATURE("android/widget/EditText");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
