/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__google__firebase_win32__hpp
#define	__libamtrs__google__firebase_win32__hpp
#include "firebase.hpp"
#pragma comment(lib, "firebase_admob.lib")
#pragma comment(lib, "firebase_app.lib")
AMTRS_GOOGLE_FIREBASE_NAMESPACE_BEGIN

ref<app> app::create()
{
	ref<app>	thiz	= new app();
	thiz->mApp				= fb::App::Create();
	return	thiz;
}

AMTRS_GOOGLE_FIREBASE_NAMESPACE_END
#endif
