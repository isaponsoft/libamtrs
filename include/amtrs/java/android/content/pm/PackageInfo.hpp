﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_content_pm_PackageInfo__hpp
#define	__libamtrs__android__java_classes__android_content_pm_PackageInfo__hpp
#include "../../../java/lang/Object.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace java::lang {
	struct	String;
}


namespace android::content::pm {

// https://developer.android.com/reference/android/content/pm/PackageItemInfo.html
AMTRS_JAVA_DEFINE_CLASS(PackageInfo , java::lang::Object)
{
	using	String				= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/content/pm/PackageItemInfo");


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
