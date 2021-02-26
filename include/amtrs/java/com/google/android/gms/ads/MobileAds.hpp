/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__com__google__android__gms__ads__MobileAds__hpp
#define	__libamtrs__android__java_classes__com__google__android__gms__ads__MobileAds__hpp
#include "../../../../../java/lang/String.hpp"
#include "../../../../../android/content/Context.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace com::google::android::gms::ads {



// https://developers.google.com/android/reference/com/google/android/gms/ads/MobileAds
AMTRS_JAVA_DEFINE_CLASS(MobileAds, java::lang::Object)
{
	using	Context	= amtrs::java::classes::android::content::Context;

	AMTRS_JAVA_CLASS_SIGNATURE("com/google/android/gms/ads/MobileAds");

	// https://developers.google.com/android/reference/com/google/android/gms/ads/MobileAds.Settings
	AMTRS_JAVA_DEFINE_CLASS(Settings , java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("com/google/android/gms/ads/MobileAds$Settings ");

		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;
		};


		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;

			AMTRS_JAVA_DEFINE_METHOD(getTrackingId
				, String()
			)

			AMTRS_JAVA_DEFINE_METHOD(isGoogleAnalyticsEnabled
				, jboolean()
			)

			AMTRS_JAVA_DEFINE_METHOD(setGoogleAnalyticsEnabled
				, Settings(jboolean enable)
			)

			AMTRS_JAVA_DEFINE_METHOD(setTrackingId
				, Settings(String trackingId)
			)
		};
	};



	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;


#if 0
		AMTRS_JAVA_DEFINE_METHOD(getRewardedVideoAdInstance
			, RewardedVideoAd(Context context)
		)
#endif

		AMTRS_JAVA_DEFINE_METHOD(initialize
			, void(Context context, String applicationCode)
			, void(Context context)
			, void(Context context, String applicationCode, Settings settings)
		)

		AMTRS_JAVA_DEFINE_METHOD(openDebugMenu
			, void(Context context, String adUnitId)
		)

		AMTRS_JAVA_DEFINE_METHOD(setAppMuted
			, void(jboolean muted)
		)

		AMTRS_JAVA_DEFINE_METHOD(setAppVolume
			, void(jfloat volume)
		)
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
