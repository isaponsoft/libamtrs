/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_content_pm_PackageManager__hpp
#define	__libamtrs__android__java_classes__android_content_pm_PackageManager__hpp
#include "../../../java/lang/Object.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace java::lang {
	struct	String;
}


namespace android::content::pm {

AMTRS_JAVA_DEFINE_CLASS(PackageManager, java::lang::Object)
{
	using	String				= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/content/pm/PackageManager");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(jint, PERMISSION_DENIED)
		AMTRS_JAVA_DEFINE_FIELD(jint, PERMISSION_GRANTED)
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
