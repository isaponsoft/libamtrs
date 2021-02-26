/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__net__HttpURLConnection__hpp
#define	__libamtrs__android__java_classes__android__net__HttpURLConnection__hpp
#include <amtrs/java/java/net/URLConnection.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::net {


AMTRS_JAVA_DEFINE_CLASS(HttpURLConnection, java::net::URLConnection)
{
	using	String	= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/net/HttpURLConnection");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

	};

	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getResponseCode
			, int()
		)

		AMTRS_JAVA_DEFINE_METHOD(setRequestMethod
			, void(String method)
		)
	};

};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
