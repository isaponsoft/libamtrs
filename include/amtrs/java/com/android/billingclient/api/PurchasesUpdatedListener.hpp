/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__com_android_billingclient_api_PurchasesUpdatedListener__hpp
#define	__libamtrs__android__java_classes__com_android_billingclient_api_PurchasesUpdatedListener__hpp
#include <amtrs/java/java/lang/Object.hpp>
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace com::android::billingclient::api {

// https://developer.android.com/reference/com/android/billingclient/api/PurchasesUpdatedListener
AMTRS_JAVA_DEFINE_CLASS(PurchasesUpdatedListener, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("com/android/billingclient/api/PurchasesUpdatedListener");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
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
