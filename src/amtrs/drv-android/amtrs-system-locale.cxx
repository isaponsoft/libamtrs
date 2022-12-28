/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/system/env.hpp>
AMTRS_SYSTEM_NAMESPACE_BEGIN

sstring get_locale_rfc3282()
{
#if 0
	JNIEnv*		env;
	JavaVM*		vm		= android::base::get_java_vm();
	bool		isNew	= language::java::util::attach(&env, vm);


	// static Locale::getLocale() を呼び出し、Locale のオブジェクトを取得します。
	jclass		jclz_Locale		= android::java::find_class(env, CURRY_ANDROID_PACKAGE "/Locale");
	jmethodID	jmid_getLocale	= env->GetStaticMethodID(jclz_Locale, "getLocale", "()L" CURRY_ANDROID_PACKAGE "/Locale;");
	language::java::util::is_exception_and_throw_logic_error(env, "drv_android_locale:on_get_language");

	jobject		jobj_locale		= env->CallStaticObjectMethod(jclz_Locale, jmid_getLocale);
	language::java::util::is_exception_and_throw_logic_error(env, "drv_android_locale:on_get_language");


	string	country		= language::java::to_string(env, (jstring)env->GetObjectField(jobj_locale, env->GetFieldID(jclz_Locale, "country",   "Ljava/lang/String;")));
	string	language	= language::java::to_string(env, (jstring)env->GetObjectField(jobj_locale, env->GetFieldID(jclz_Locale, "language",  "Ljava/lang/String;")));

	// 取得した Locale の参照を開放
	env->DeleteLocalRef(jobj_locale);

	if (isNew)
	{
		vm->DetachCurrentThread();
	}


	return	language + "-" + country;
#endif
	return	"";
};


AMTRS_SYSTEM_NAMESPACE_END
