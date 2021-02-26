/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__support__v4__app__FragmentActivity__hpp
#define	__libamtrs__android__java_classes__android__support__v4__app__FragmentActivity__hpp
#include <amtrs/java/android/app/Activity.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::support::v4::app {

struct	FragmentManager;

// https://developer.android.com/reference/android/support/v4/app/FragmentActivity
AMTRS_JAVA_DEFINE_CLASS(FragmentActivity, android::app::Activity)
{
	using	FragmentManager		= android::support::v4::app::FragmentManager;


	AMTRS_JAVA_CLASS_SIGNATURE("android/support/v4/app/FragmentActivity");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getSupportFragmentManager
			, FragmentManager()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
