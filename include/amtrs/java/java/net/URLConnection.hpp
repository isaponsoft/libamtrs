/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__java__jc__java__net__URLConnection__hpp
#define	__libamtrs__java__jc__java__net__URLConnection__hpp
#include <amtrs/java/def.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::io {
	struct	InputStream;
	struct	OutputStream;
}
namespace java::net {

struct	URL;

// https://developer.android.com/reference/java/net/URLConnection
AMTRS_JAVA_DEFINE_CLASS(URLConnection, java::lang::Object)
{
	using	String			= java::lang::String;
	using	URL				= java::net::URL;
	using	InputStream		= java::io::InputStream;
	using	OutputStream	= java::io::OutputStream;

	AMTRS_JAVA_CLASS_SIGNATURE("java/net/URLConnection");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};

	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(jboolean, allowUserInteraction)
		AMTRS_JAVA_DEFINE_FIELD(jboolean, connected)
		AMTRS_JAVA_DEFINE_FIELD(jboolean, doInput)
		AMTRS_JAVA_DEFINE_FIELD(jboolean, doOutput)
		AMTRS_JAVA_DEFINE_FIELD(jlong,    ifModifiedSince)
		AMTRS_JAVA_DEFINE_FIELD(URL,      url)
		AMTRS_JAVA_DEFINE_FIELD(jboolean, useCaches )


		AMTRS_JAVA_DEFINE_METHOD(connect,
			void()
		)

		AMTRS_JAVA_DEFINE_METHOD(disconnect,
			void()
		)

		AMTRS_JAVA_DEFINE_METHOD(getContentEncoding,
			String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getContentLength,
			int()
		)

		AMTRS_JAVA_DEFINE_METHOD(getContentLengthLong,
			jlong()
		)

		AMTRS_JAVA_DEFINE_METHOD(getContentType,
			String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getIfModifiedSince,
			jlong()
		)

		AMTRS_JAVA_DEFINE_METHOD(getHeaderField
			, String(String name)
			, String(int n)
		)

		AMTRS_JAVA_DEFINE_METHOD(getHeaderFieldKey
			, String(int n)
		)

		AMTRS_JAVA_DEFINE_METHOD(getInputStream,
			InputStream()
		)

		AMTRS_JAVA_DEFINE_METHOD(getLastModified,
			jlong()
		)

		AMTRS_JAVA_DEFINE_METHOD(getOutputStream,
			OutputStream()
		)

		AMTRS_JAVA_DEFINE_METHOD(getURL,
			URL()
		)

		AMTRS_JAVA_DEFINE_METHOD(setReadTimeout,
			void(int timeout)
		)

		AMTRS_JAVA_DEFINE_METHOD(setRequestProperty,
			void(java::lang::String key, java::lang::String value)
		)

		AMTRS_JAVA_DEFINE_METHOD(setUseCaches,
			void(jboolean usecaches)
		)
	};

};



}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
