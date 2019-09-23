/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_content_ContextWrapper_hpp
#define	__libamtrs__android__java_classes__android_content_ContextWrapper_hpp
#include <amtrs/java/android/content/Context.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::content {

// https://developer.android.com/reference/android/app/Activity
AMTRS_JAVA_DEFINE_CLASS(ContextWrapper, android::content::Context)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/content/ContextWrapper");


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
