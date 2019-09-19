﻿/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android__support__v4__Fragment__hpp
#define	__libamtrs__android__java_classes__android__support__v4__Fragment__hpp
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::support::v4::app {

// https://developer.android.com/reference/android/support/v4/app/Fragment.html
AMTRS_JAVA_DEFINE_CLASS(Fragment, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/support/v4/app/Fragment");


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
