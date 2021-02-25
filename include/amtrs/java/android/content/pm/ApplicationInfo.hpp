/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_content_pm_ApplicationInfo__hpp
#define	__libamtrs__android__java_classes__android_content_pm_ApplicationInfo__hpp
#include "../../../java/lang/Object.hpp"
#include "PackageInfo.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace java::lang {
	struct	String;
}


namespace android::content::pm {

// https://developer.android.com/reference/android/content/pm/ApplicationInfo.html#uid
AMTRS_JAVA_DEFINE_CLASS(ApplicationInfo , android::content::pm::PackageInfo)
{
	using	String				= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/content/pm/ApplicationInfo");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(String	, backupAgentName)
		AMTRS_JAVA_DEFINE_FIELD(int		, category)
		AMTRS_JAVA_DEFINE_FIELD(String	, className)
		AMTRS_JAVA_DEFINE_FIELD(int		, compatibleWidthLimitDp)
		AMTRS_JAVA_DEFINE_FIELD(String	, dataDir)
		AMTRS_JAVA_DEFINE_FIELD(int		, descriptionRes)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
