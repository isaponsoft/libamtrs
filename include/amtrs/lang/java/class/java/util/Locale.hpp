/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__java__util__Locale__hpp
#define	__libamtrs__android__java_classes__java__util__Locale__hpp
#include "../../java/lang/Enum.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace java::util {

// https://developer.android.com/reference/java/util/Locale
AMTRS_JAVA_DEFINE_CLASS(Locale, java::lang::Object)
{
	using	String				= java::lang::String;
	AMTRS_JAVA_CLASS_SIGNATURE("java/util/Locale");


	AMTRS_JAVA_DEFINE_CLASS(Category, java::lang::Enum)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("java/util/Locale$Category");

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
			, Locale(Category)
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
