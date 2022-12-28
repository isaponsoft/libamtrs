/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_contentres_Resources__hpp
#define	__libamtrs__android__java_classes__android_contentres_Resources__hpp
#include "../../../java/lang/Object.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace java::lang {
	struct	String;
}

namespace android::util {
	struct	DisplayMetrics;
}


namespace android::content::res {

struct	AssetManager;
struct	Configuration;

// https://developer.android.com/reference/android/content/res/Resources.html
AMTRS_JAVA_DEFINE_CLASS(Resources, java::lang::Object)
{
	using	AssetManager		= android::content::res::AssetManager;
	using	String				= java::lang::String;
	using	DisplayMetrics		= android::util::DisplayMetrics;

	AMTRS_JAVA_CLASS_SIGNATURE("android/content/res/Resources");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getSystem,
			Resources()
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getAssets,
			AssetManager()
		)

		AMTRS_JAVA_DEFINE_METHOD(getConfiguration,
			Configuration()
		)

		AMTRS_JAVA_DEFINE_METHOD(getDisplayMetrics,
			DisplayMetrics()
		)

		AMTRS_JAVA_DEFINE_METHOD(getResourceEntryName,
			String(jint resid)
		)

		AMTRS_JAVA_DEFINE_METHOD(getResourceName,
			String(jint resid)
		)

		AMTRS_JAVA_DEFINE_METHOD(getResourcePackageName,
			String(jint resid)
		)

		AMTRS_JAVA_DEFINE_METHOD(getResourceTypeName,
			String(jint resid)
		)

		AMTRS_JAVA_DEFINE_METHOD(getString,
			String(jint id)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
