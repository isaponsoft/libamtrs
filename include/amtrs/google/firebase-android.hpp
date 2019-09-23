/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
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
