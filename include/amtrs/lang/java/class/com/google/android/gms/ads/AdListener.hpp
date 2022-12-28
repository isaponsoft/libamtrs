/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__com__google__android__gms__ads__AdListener__hpp
#define	__libamtrs__android__java_classes__com__google__android__gms__ads__AdListener__hpp
#include "../../../../../java/lang/String.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace com::google::android::gms::ads {


// https://developers.google.com/android/reference/com/google/android/gms/ads/AdListener
AMTRS_JAVA_DEFINE_CLASS(AdListener, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("com/google/android/gms/ads/AdListener");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(onAdClicked
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(onAdClosed
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(onAdFailedToLoad
			, void(int errorCode)
		)

		AMTRS_JAVA_DEFINE_METHOD(onAdImpression
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(onAdLeftApplication
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(onAdLoaded
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(onAdOpened
			, void()
		)
	};
};

}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
