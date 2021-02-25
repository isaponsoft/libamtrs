/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__java__init__hpp
#define	__libamtrs__java__init__hpp
AMTRS_JAVA_NAMESPACE_BEGIN


// ***************************************************************************
//! クラスからオブジェクトを生成します。
// ---------------------------------------------------------------------------
//!	class	JavaClass {
//!		public JavaClass(Context);
//!		public JavaClass(int, int);
//!
//!	using		init	= call_init<JavaClass, void(Context), void(jint,jint)>;
//!
//!	jnienv*			env;
//!	jcls<JavaClass>	cls;
//!	jobj<Context>	param1;
//!
//!	jobj<JavaClass>	retval	= init{}(env, (jclass)cls, param1);
// ---------------------------------------------------------------------------
template<class T, class...>
struct	call_init;


template<class T, class R, class... Args>
struct	call_init<T, R(Args...)>
{
	jobj<T> operator () (JNIEnv* _env, jclass _cls, typename jni_traits<Args>::argv_type... _args)
	{
		jmethodID	mid	= _env->GetMethodID(_cls, "<init>", jni_traits<void(Args...)>::signature().c_str());
		jobject		obj	= _env->NewObject(_cls, mid, ((typename jni_traits<Args>::jni_type)_args)...);
		return	jobj<T>(obj);
	}
};



template<class T, class R, class... Args, class... OtherFuncT>
struct	call_init<T, R(Args...), OtherFuncT...>
		: call_init<T, R(Args...)>
		, call_init<T, OtherFuncT...>
{
	using	base_type_1		= call_init<T, R(Args...)>;
	using	base_type_2		= call_init<T, OtherFuncT...>;
	using	base_type_1::operator ();
	using	base_type_2::operator ();
};



AMTRS_JAVA_NAMESPACE_END
#endif
