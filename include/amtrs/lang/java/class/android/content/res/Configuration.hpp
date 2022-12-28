/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_contentres_Configuration__hpp
#define	__libamtrs__android__java_classes__android_contentres_Configuration__hpp
#include "../../../java/lang/Object.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace java::lang {
	struct	String;
}

namespace android::util {
	struct	Locale;
}


namespace android::content::res {


// https://developer.android.com/reference/android/content/res/Resources.html
AMTRS_JAVA_DEFINE_CLASS(Configuration, java::lang::Object)
{
	using	String			= java::lang::String;
	using	Locale			= android::util::Locale;

	AMTRS_JAVA_CLASS_SIGNATURE("android/content/res/Configuration");


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
