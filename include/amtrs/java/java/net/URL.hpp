/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__java__jc__java__net__URL__hpp
#define	__libamtrs__java__jc__java__net__URL__hpp
#include <amtrs/java/java/lang.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::net {

struct	URLConnection;
struct	URLStreamHandler;

// https://developer.android.com/reference/java/net/URL
AMTRS_JAVA_DEFINE_CLASS(URL, java::lang::Object)
{
	using	String				= java::lang::String;
	using	URLConnection		= java::net::URLConnection;

	AMTRS_JAVA_CLASS_SIGNATURE("java/net/URL");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;


		AMTRS_JAVA_DEFINE_INIT(
			  URL(String protocol, String host, jint port, String file)
			, URL(String protocol, String host, String file)
			, URL(String protocol, String host, String file, URLStreamHandler handler)
			, URL(String spec)
			, URL(URL context, String spec)
			, URL(URL context, String spec, URLStreamHandler handler)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(openConnection
			, URLConnection()
		)


		AMTRS_JAVA_DEFINE_METHOD(getContentLength
			, int()
		)

		AMTRS_JAVA_DEFINE_METHOD(getContentLengthLong
			, jlong()
		)

		AMTRS_JAVA_DEFINE_METHOD(getContentType
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getLastModified
			, jlong()
		)

	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
