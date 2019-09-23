/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_contentres_Resources__hpp
#define	__libamtrs__android__java_classes__android_contentres_Resources__hpp
#include <amtrs/java/def.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace java::lang {
	struct	String;
}

namespace android::util {
	struct	DisplayMetrics;
}


namespace android::content::res {

struct	AssetManager;

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
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getAssets,
			AssetManager()
		)

		AMTRS_JAVA_DEFINE_METHOD(getDisplayMetrics,
			DisplayMetrics()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
