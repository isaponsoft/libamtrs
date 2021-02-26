/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net__simple_http_android__hpp
#define	__libamtrs__net__simple_http_android__hpp
#include "../java/java/net/URL.hpp"
#include "../java/java/net/URLConnection.hpp"
#include "../java/java/io/InputStream.hpp"
#include "../java/java/io/OutputStream.hpp"
#include "../java/android/net/HttpURLConnection.hpp"
#include "android-api-android_app.hpp"
AMTRS_NET_NAMESPACE_BEGIN


class	simple_http_android
		: public simple_http
{
public:
	//! 指定したURLへ GET のリクエストを行います。
	virtual void request_get(std::string_view _url) override
	{
		auto	url		= java::to_jstring(_url);
		auto	jurl	= java::classes::java::net::URL::find().init(java::jobj<java::classes::java::lang::String>(url));
		auto	conn	= jurl.openConnection();
		auto	http	= java::jobj<java::classes::android::net::HttpURLConnection>(java::local_obj(conn.get()));

		auto	s		= java::jobj<java::classes::java::lang::String>(java::to_jstring("GET"));
		http.setRequestMethod(s);
		//http.setRequestProperty(java::to_jstring("Content-Type").get(), java::to_jstring("application/json; charset=utf-8").get());

		mContentLength		= http.getContentLength();
		mResponseCode		= http.getResponseCode();
		mContentType		= std::to_string((jstring)http.getContentType().get());

		for (int i = 0; ; ++i)
		{
			auto	key		= http.getHeaderFieldKey(i);
			auto	value	= http.getHeaderField(i);
			auto	k		= std::to_string((jstring)key.get());
			auto	v		= std::to_string((jstring)value.get());
			if (!key.get() && !value.get())
			{
				break;
			}
			mHeaders.push_back(value_type{std::move(k), std::move(v)});
		}

		auto	inStream	= http.getInputStream();
		mContentBody		= inStream.recv_to_string();
		inStream.close();

		conn.disconnect();

		AMTRS_DEBUG_LOG("HTTP : code=%d, content-length=%d, content-type=%s", mResponseCode, (int)mContentLength, mContentType.c_str());
	}

	friend	class simple_http;
};

inline ref<simple_http> simple_http::create()
{
	ref<simple_http_android>	thiz	= new simple_http_android();
	return	thiz;
}



AMTRS_NET_NAMESPACE_END
#endif
