/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__com_android_billingclient_api_SkuDetails__hpp
#define	__libamtrs__android__java_classes__com_android_billingclient_api_SkuDetails__hpp
#include <amtrs/java/java/lang/Object.hpp>
#include <amtrs/java/java/lang/String.hpp>
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace com::android::billingclient::api {

// https://developer.android.com/reference/com/android/billingclient/api/PurchasesUpdatedListener
AMTRS_JAVA_DEFINE_CLASS(SkuDetails, java::lang::Object)
{
	using	String	= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("com/android/billingclient/api/SkuDetails");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getDescription 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getFreeTrialPeriod 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getIconUrl 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getIntroductoryPrice 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getIntroductoryPriceAmountMicros 
			, jlong()
		)

		AMTRS_JAVA_DEFINE_METHOD(getIntroductoryPriceCycles 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getIntroductoryPricePeriod 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getOriginalJson 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getOriginalPrice 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getOriginalPriceAmountMicros 
			, jlong()
		)

		AMTRS_JAVA_DEFINE_METHOD(getPrice 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getPriceAmountMicros 
			, jlong()
		)

		AMTRS_JAVA_DEFINE_METHOD(getPriceCurrencyCode 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getSku 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getSubscriptionPeriod 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getTitle 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getType 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(isRewarded 
			, jboolean()
		)

	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
