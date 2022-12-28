/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_os_Bundle__hpp
#define	__libamtrs__android__java_classes__android_os_Bundle__hpp
#include "../../java/lang/String.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::os {

// https://developer.android.com/reference/android/os/BaseBundle.html
AMTRS_JAVA_DEFINE_CLASS(BaseBundle, java::lang::Object)
{
	using	String	= java::lang::String;

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


		AMTRS_JAVA_DEFINE_METHOD(putBoolean
			, void(String key, jboolean value)
		)
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


		AMTRS_JAVA_DEFINE_INIT(
			  Bundle()
		)
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
