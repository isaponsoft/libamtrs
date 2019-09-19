/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__com_android_billingclient_api_BillingResult__hpp
#define	__libamtrs__android__java_classes__com_android_billingclient_api_BillingResult__hpp
#include <amtrs/java/java/lang/Object.hpp>
#include <amtrs/java/java/lang/String.hpp>
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace com::android::billingclient::api {

// https://developer.android.com/reference/com/android/billingclient/api/BillingResult
AMTRS_JAVA_DEFINE_CLASS(BillingResult, java::lang::Object)
{
	using	String	= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("com/android/billingclient/api/BillingResult");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getDebugMessage 
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getResponseCode 
			, jint()
		)

	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
