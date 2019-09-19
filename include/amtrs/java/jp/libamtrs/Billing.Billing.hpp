/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__jp_libamtrs_Billing_Billing__hpp
#define	__libamtrs__android__java_classes__jp_libamtrs_Billing_Billing__hpp
#include <amtrs/java/java/util/List.hpp>
#include <amtrs/java/com/android/billingclient/api/PurchasesUpdatedListener.hpp>
#include <amtrs/java/android/app/Activity.hpp>
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::app {
struct	Activity;
}

namespace jp::libamtrs::Billing {

struct	SkuDetails;

// https://developer.android.com/reference/android/app/FragmentActivity
AMTRS_JAVA_DEFINE_CLASS(Billing, com::android::billingclient::api::PurchasesUpdatedListener)
{
	using	Activity	= android::app::Activity;
	using	List		= java::util::List;
	using	String		= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("jp/libamtrs/Billing/Billing");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_INIT(
			  Billing(jlong _native, Activity _activity)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		// Billingｈ接続を開始します。
		AMTRS_JAVA_DEFINE_METHOD(startConnection
			, void()
		)

		// SKUの詳細を取得します。
		AMTRS_JAVA_DEFINE_METHOD(loadSkuDetails
			, void(List _skuList)
		)

		AMTRS_JAVA_DEFINE_METHOD(onSkuDetails
			, void(jlong _nativeHandle, SkuDetails _details)
		)

		AMTRS_JAVA_DEFINE_METHOD(startPurchase
			, jboolean(String _sku)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
