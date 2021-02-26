/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__com__google__android__gms__ads__AdRequest__hpp
#define	__libamtrs__android__java_classes__com__google__android__gms__ads__AdRequest__hpp
#include "../../../../../java/lang/String.hpp"
#include "../../../../../android/content/Context.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::util {
	struct	Date;
}
namespace android::location {
	struct	Location;
}


namespace com::google::android::gms::ads {


// https://developers.google.com/android/reference/com/google/android/gms/ads/AdRequest
AMTRS_JAVA_DEFINE_CLASS(AdRequest, java::lang::Object)
{
	using	Context		= amtrs::java::classes::android::content::Context;
	using	Location	= amtrs::java::classes::android::location::Location;
	using	Date		= java::util::Date;
	using	String		= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("com/google/android/gms/ads/AdRequest");


	AMTRS_JAVA_DEFINE_CLASS(Builder, java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("com/google/android/gms/ads/AdRequest$Builder");


		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;

			AMTRS_JAVA_DEFINE_INIT(
				  void()
			)
		};


		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;

			AMTRS_JAVA_DEFINE_METHOD(build
				, AdRequest()
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


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
