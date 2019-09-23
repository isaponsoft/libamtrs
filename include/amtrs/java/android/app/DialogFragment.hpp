/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_app_DialogFragment__hpp
#define	__libamtrs__android__java_classes__android_app_DialogFragment__hpp
#include <amtrs/java/android/app/Fragment.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::app {

struct	FragmentManager;
struct	FragmentTransaction;

// https://developer.android.com/reference/android/app/DialogFragment
AMTRS_JAVA_DEFINE_CLASS(DialogFragment, Fragment)
{
	using	String				= java::lang::String;
	using	FragmentManager		= android::app::FragmentManager;
	using	FragmentTransaction	= android::app::FragmentTransaction;

	AMTRS_JAVA_CLASS_SIGNATURE("android/app/DialogFragment");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(show
			, void(FragmentManager manager, String tag)
//			, jint(FragmentTransaction transaction, String tag)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
