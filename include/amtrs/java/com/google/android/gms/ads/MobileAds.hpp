/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__com__google__android__gms__ads__hpp
#define	__libamtrs__android__java_classes__com__google__android__gms__ads__hpp
#include <amtrs/java/def.hpp>

#include <amtrs/java/android/content/Context.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::util {
	struct	Date;
}
namespace android::location {
	struct	Location;
}


namespace com::google::android::gms::ads {

using namespace java::lang;

struct	AdListener;
struct	AdRequest;
struct	AdSize;


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


// https://developers.google.com/android/reference/com/google/android/gms/ads/AdRequest
AMTRS_JAVA_DEFINE_CLASS(AdRequest, java::lang::Object)
{
	using	Context		= amtrs::java::classes::android::content::Context;
	using	Location	= amtrs::java::classes::android::location::Location;
	using	Date		= java::util::Date;

	AMTRS_JAVA_CLASS_SIGNATURE("com/google/android/gms/ads/AdRequest");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getBirthday
			, Date()
		)

		AMTRS_JAVA_DEFINE_METHOD(getContentUrl
			, String()
		)

/*		AMTRS_JAVA_DEFINE_METHOD(getCustomEventExtrasBundle
			, <T extends CustomEvent> Bundle (Class<T> adapterClass)
		)*/

		AMTRS_JAVA_DEFINE_METHOD(getGender
			, int()
		)

/*		AMTRS_JAVA_DEFINE_METHOD(getKeywords
			, Set<String>()
		)*/

		AMTRS_JAVA_DEFINE_METHOD(getLocation
			, Location()
		)

/*		AMTRS_JAVA_DEFINE_METHOD(getNetworkExtras
			, <T extends NetworkExtras> T (Class<T> networkExtrasClass)
		)*/

/*		AMTRS_JAVA_DEFINE_METHOD(getNetworkExtrasBundle
			, <T extends MediationAdapter> Bundle  (Class<T> adapterClass)
		)*/

		AMTRS_JAVA_DEFINE_METHOD(isTestDevice
			, jboolean(Context context)
		)
	};
};


// https://developers.google.com/android/reference/com/google/android/gms/ads/AdSize
AMTRS_JAVA_DEFINE_CLASS(AdSize, java::lang::Object)
{
	using	Context	= amtrs::java::classes::android::content::Context;

	AMTRS_JAVA_CLASS_SIGNATURE("com/google/android/gms/ads/AdSize");

	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(equals
			, jboolean(Object)
		)

		AMTRS_JAVA_DEFINE_METHOD(getHeight
			, int()
		)

		AMTRS_JAVA_DEFINE_METHOD(getHeightInPixels
			, int(Context context)
		)

		AMTRS_JAVA_DEFINE_METHOD(getWidth
			, int()
		)

		AMTRS_JAVA_DEFINE_METHOD(getWidthInPixels
			, int(Context context)
		)

		AMTRS_JAVA_DEFINE_METHOD(hashCode
			, int()
		)

		AMTRS_JAVA_DEFINE_METHOD(isAutoHeight
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(isFluid
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(isFullWidth
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(toString
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(setAdSize
			, void(AdSize adSize)
		)

		AMTRS_JAVA_DEFINE_METHOD(setAdUnitId
			, void(String adUnitId)
		)

	// Protected method.
		AMTRS_JAVA_DEFINE_METHOD(onLayout
			, void(jboolean changed, int left, int top, int right, int bottom)
		)

		AMTRS_JAVA_DEFINE_METHOD(onMeasure
			, void(int widthMeasureSpec, int heightMeasureSpec)
		)
	};


};


// https://developers.google.com/android/reference/com/google/android/gms/ads/AdView
AMTRS_JAVA_DEFINE_CLASS(AdView, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("com/google/android/gms/ads/AdView");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(destroy
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(getAdListener
			, AdListener()
		)

		AMTRS_JAVA_DEFINE_METHOD(getAdSize
			, AdSize()
		)

		AMTRS_JAVA_DEFINE_METHOD(getAdUnitId
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getMediationAdapterClassName
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(isLoading
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(loadAd
			, void(AdRequest adRequest)
		)

		AMTRS_JAVA_DEFINE_METHOD(pause
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(resume
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(setAdListener
			, void(AdListener adListener)
		)

		AMTRS_JAVA_DEFINE_METHOD(setAdSize
			, void(AdSize adSize)
		)

		AMTRS_JAVA_DEFINE_METHOD(setAdUnitId
			, void(String adUnitId)
		)

	// Protected method.
		AMTRS_JAVA_DEFINE_METHOD(onLayout
			, void(jboolean changed, int left, int top, int right, int bottom)
		)

		AMTRS_JAVA_DEFINE_METHOD(onMeasure
			, void(int widthMeasureSpec, int heightMeasureSpec)
		)
	};


};


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
