/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__net__Uri__hpp
#define	__libamtrs__android__java_classes__android__net__Uri__hpp
#include "../../java/lang/String.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::net {


AMTRS_JAVA_DEFINE_CLASS(Uri, java::lang::Object)
{
	using	String	= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/net/Uri");

	AMTRS_JAVA_DEFINE_CLASS(Builder, java::lang::Object)
	{
		using	String	= java::lang::String;

		AMTRS_JAVA_CLASS_SIGNATURE("android/net/Uri$Builder");


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

		AMTRS_JAVA_DEFINE_METHOD(parse
			, Uri(String uriString)
		)

	};

	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getAuthority
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getFragment
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getHost
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getLastPathSegment
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getPath
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getScheme
			, String()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
