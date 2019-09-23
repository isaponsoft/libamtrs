/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_os_Bundle__hpp
#define	__libamtrs__android__java_classes__android_os_Bundle__hpp
#include <amtrs/java/def.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::os {

// https://developer.android.com/reference/android/os/BaseBundle.html
AMTRS_JAVA_DEFINE_CLASS(BaseBundle, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/os/BaseBundle");


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


// https://developer.android.com/reference/android/os/Bundle.html
AMTRS_JAVA_DEFINE_CLASS(Bundle, BaseBundle)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/os/Bundle");


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
