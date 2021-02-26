/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__google__firebase_android__hpp
#define	__libamtrs__google__firebase_android__hpp
#include "firebase.hpp"
AMTRS_GOOGLE_FIREBASE_NAMESPACE_BEGIN

ref<app> app::create()
{
	ref<app>	thiz	= new app();
	JNIEnv*		env;
	auto*		activity	= os::android::get_native_activity();
	activity->vm->AttachCurrentThread(&env, nullptr);
	auto		act			= env->NewLocalRef(activity->clazz);
	thiz->mApp				= fb::App::Create(env, act);
	return	thiz;
}

AMTRS_GOOGLE_FIREBASE_NAMESPACE_END
#endif
