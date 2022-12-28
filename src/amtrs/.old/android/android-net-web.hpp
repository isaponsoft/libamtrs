/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__web__web_android__hpp
#define	__libamtrs__web__web_android__hpp
#include "android-api-android_app.hpp"
#include "../java/@"
#include "../java/android/content/Context.hpp"
#include "../java/android/content/Intent.hpp"
#include "../java/android/net/Uri.hpp"
#include "../java/android/os/Bundle.hpp"
AMTRS_NAMESPACE_BEGIN

struct	web
{
	static void jump_to(const std::string& _url)
	{
		using	String	= java::classes::java::lang::String;
		using	Intent	= java::classes::android::content::Intent;
		using	Uri		= java::classes::android::net::Uri;

		os::android::get_activity().startActivity
		(
			Intent::init(
				Intent::find().get_ACTION_VIEW(),
				Uri::find().parse(java::jobj<String>(java::to_jstring(_url)))
			)
		);
	}
};



AMTRS_NAMESPACE_END
#endif
