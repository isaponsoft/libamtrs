/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_util__Locale__hpp
#define	__libamtrs__android__java_classes__android_util__Locale__hpp
#include <amtrs/java/def.hpp>
#include <amtrs/java/java/lang/Enum.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::util {

// https://developer.android.com/reference/java/util/Locale
AMTRS_JAVA_DEFINE_CLASS(Locale, java::lang::Object)
{
	using	String				= java::lang::String;


	AMTRS_JAVA_DEFINE_CLASS(Locale, java::lang::Enum)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/util/Locale/Category");

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
	}


	AMTRS_JAVA_CLASS_SIGNATURE("android/util/Locale");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_INIT(
			  Locale(String language, String country, String variant)
			, Locale(String language, String country)
			, Locale(String language)
		)

		AMTRS_JAVA_DEFINE_METHOD(getDefault
			, Locale(Categoty)
			, Locale()
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getCountry
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getDisplayCountry
			, String(Locale locale)
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getDisplayLanguage
			, String(Locale locale)
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getDisplayName
			, String(Locale locale)
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getDisplayScript
			, String(Locale inLocale)
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getISO3Country
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getISO3Language
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getLanguage
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getScript
			, String()
		)


	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
