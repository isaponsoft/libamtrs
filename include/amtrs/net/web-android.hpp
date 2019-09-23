/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__web__web_android__hpp
#define	__libamtrs__web__web_android__hpp
#include "../amtrs.hpp"
#include "../java/@"
#include "../os/android/android_app.hpp"
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
