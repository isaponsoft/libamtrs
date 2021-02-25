/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__com_android_billingclient_api_Purchase__hpp
#define	__libamtrs__android__java_classes__com_android_billingclient_api_Purchase__hpp
#include <amtrs/java/java/lang/Object.hpp>
#include <amtrs/java/java/lang/String.hpp>
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace java::util {
struct	List;
}

namespace com::android::billingclient::api {

struct	BillingResult;

// https://developer.android.com/reference/com/android/billingclient/api/Purchase
AMTRS_JAVA_DEFINE_CLASS(Purchase, java::lang::Object)
{
	using	String	= java::lang::String;
	using	List	= java::util::List;

	AMTRS_JAVA_CLASS_SIGNATURE("com/android/billingclient/api/Purchase");


	AMTRS_JAVA_DEFINE_CLASS(PurchaseState, classes::java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("com/android/billingclient/api/Purchase$PurchaseState");

		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;

			AMTRS_JAVA_DEFINE_FIELD(jint, PENDING)
			AMTRS_JAVA_DEFINE_FIELD(jint, PURCHASED)
			AMTRS_JAVA_DEFINE_FIELD(jint, UNSPECIFIED_STATE)
		};

		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;
		};
	};


	AMTRS_JAVA_DEFINE_CLASS(PurchasesResult, classes::java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("com/android/billingclient/api/Purchase$PurchasesResult");

		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;
		};

		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;

			AMTRS_JAVA_DEFINE_METHOD(getBillingResult 
				, BillingResult()
			)

			AMTRS_JAVA_DEFINE_METHOD(getPurchasesList 
				, List()
			)

			AMTRS_JAVA_DEFINE_METHOD(getResponseCode 
				, jint()
			)
		};
	};


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getDeveloperPayload 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getOrderId 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getOriginalJson 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getPackageName 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getPurchaseState 
			, jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(getPurchaseTime 
			, jlong()
		)

		AMTRS_JAVA_DEFINE_METHOD(getPurchaseToken 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getSignature 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getSku 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(isAcknowledged 
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(isAutoRenewing 
			, jboolean()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
