/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
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
