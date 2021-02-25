/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__java__call_method__hpp
#define	__libamtrs__java__call_method__hpp
AMTRS_JAVA_NAMESPACE_BEGIN



// ****************************************************************************
//! 静的メソッドを呼び出します。
// ----------------------------------------------------------------------------
//! テンプレート引数で指定する関数型を持つ静的メソッドを呼び出します。
//!
//!	jstring											s;
//! call_static_method<jint(java::lang::String)>	c();
//!	c(jnienv, clazz, methodId, s);
// ****************************************************************************
template<class...>
struct	call_static_method;		// Prototype. No use.



// ****************************************************************************
//! 動的メソッドを呼び出します。
// ----------------------------------------------------------------------------
//! テンプレート引数で指定する関数型を持つ動的メソッドを呼び出します。
//!
//!	jstring									s;
//! call_method<jint(java::lang::String)>	c();
//!	c(jnienv, obj, methodId, s);
// ****************************************************************************
template<class...>
struct	call_method;			// Prototype. No use.



// ****************************************************************************
//! 静的／動的メソッドを呼び出します。
// ----------------------------------------------------------------------------
//! テンプレート引数で指定する関数型を持つ動的メソッドを呼び出します。
//!	jobject を渡すか jclass によって、call_static_method か call_method を
//!	呼び分けます。
//!
//!	jstring								s;
//! caller<jint(java::lang::String)>	c();
//!	c(jnienv, obj, methodId, s);
// ****************************************************************************
template<class...>
struct	caller;					// Prototype. No use.


// ****************************************************************************
//! オーバーロードされたメソッド呼び出しを行います。
// ----------------------------------------------------------------------------
//!	このクラスは、自身でシグネチャの解析を行います。
//!	func1_type	= FontMetrics();
//!	func2_type	= float(FontMetrics);
//!
//! overload_call<func1_type, func2_typ)>	c();
//!	c(jnienv, obj, cls, s);
// ****************************************************************************
template<class...>
struct	overload_call;





template<class R, class... Args, class... OtherFuncT>
struct	overload_call<R(Args...), OtherFuncT...>
		: overload_call<R(Args...)>
		, overload_call<OtherFuncT...>
{
	using	base_type_1		= overload_call<R(Args...)>;
	using	base_type_2		= overload_call<OtherFuncT...>;
	using	base_type_1::operator ();
	using	base_type_2::operator ();
};





template<class R, class... Args>
struct	call_static_method<R(Args...)>
{
	typename jni_traits<R>::jni_type operator () (JNIEnv* _env, jclass _cls, jmethodID _mid, typename jni_traits<Args>::jni_type... _args)
	{
		using	jni_type	= typename jni_traits<R>::jni_type;
		if constexpr (std::is_same<jni_type, void>::value)
		{
			_env->CallStaticVoidMethod(_cls, _mid, _args...);
			return;
		}
		else if constexpr (std::is_same<jni_type, jbyte   >::value)		return	_env->CallStaticByteMethod		(_cls, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jshort  >::value)		return	_env->CallStaticShortMethod		(_cls, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jint    >::value)		return	_env->CallStaticIntMethod		(_cls, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jlong   >::value)		return	_env->CallStaticLongMethod		(_cls, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jboolean>::value)		return	_env->CallStaticBooleanMethod	(_cls, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jchar   >::value)		return	_env->CallStaticCharMethod		(_cls, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jfloat  >::value)		return	_env->CallStaticFloatMethod		(_cls, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jobject >::value)		return	_env->CallStaticObjectMethod	(_cls, _mid, _args...);
	}
};



template<class R, class... Args>
struct	call_method<R(Args...)>
{
	typename jni_traits<R>::jni_type operator () (JNIEnv* _env, jobject _obj, jmethodID _mid, typename jni_traits<Args>::jni_type... _args)
	{
		using	jni_type	= typename jni_traits<R>::jni_type;
		if constexpr (std::is_same<jni_type, void>::value)
		{
			_env->CallVoidMethod(_obj, _mid, _args...);
			return;
		}
		else if constexpr (std::is_same<jni_type, jbyte       >::value)		return	_env->CallByteMethod		(_obj, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jshort      >::value)		return	_env->CallShortMethod		(_obj, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jint        >::value)		return	_env->CallIntMethod			(_obj, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jlong       >::value)		return	_env->CallLongMethod		(_obj, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jboolean    >::value)		return	_env->CallBooleanMethod		(_obj, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jchar       >::value)		return	_env->CallCharMethod		(_obj, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jfloat      >::value)		return	_env->CallFloatMethod		(_obj, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jobject     >::value)		return	_env->CallObjectMethod		(_obj, _mid, _args...);
		else if constexpr (std::is_same<jni_type, jobjectArray>::value)		return	(jobjectArray)_env->CallObjectMethod(_obj, _mid, _args...);
	}
};



template<class R, class... Args>
struct	caller<R(Args...)>
{
	template<class ClzOrObj>
	auto operator () (JNIEnv* _env, ClzOrObj _owner, jmethodID _mid, typename jni_traits<Args>::jni_type... _args)
	{
		return	typename std::conditional<std::is_same<ClzOrObj, jclass>::value, call_static_method<R(Args...)>, call_method<R(Args...)>>::type{}
				(_env, _owner, _mid, _args...);
	}
};



template<class R, class... Args>
struct	overload_call<R(Args...)>
{
	static constexpr auto	signature() noexcept { return jni_traits<R(Args...)>::signature(); }

	// static method
	auto operator () (const char* _name, JNIEnv* _env, jclass _cls, typename jni_traits<Args>::argv_type... _args)
	{
		using	return_type	= typename jni_traits<R>::jni_type;

		// まずはメソッドIDを取得
		jmethodID	mid	= _env->GetStaticMethodID(_cls, _name, signature().c_str());

		// 戻り値の種類によって呼び出し関数を変更する
		if constexpr (std::is_same<return_type, jobject>::value)
		{
			return	jobj<R>(local_obj::make(caller<R(Args...)>{}(_env, _cls, mid, ((typename jni_traits<Args>::jni_type)_args)...)));
		}
		else if constexpr (std::is_same<return_type, void>::value)
		{
			caller<R(Args...)>{}(_env, _cls, mid, ((typename jni_traits<Args>::jni_type)_args)...);
			return;
		}
		else if constexpr (!std::is_same<return_type, jobject>::value)
		{
			return	caller<R(Args...)>{}(_env, _cls, mid, ((typename jni_traits<Args>::jni_type)_args)...);
		}
	}

	// dynamic method
	auto operator () (const char* _name, JNIEnv* _env, jobject _obj, jclass _cls, typename jni_traits<Args>::argv_type... _args)
	{
		jmethodID	mid	= _env->GetMethodID(_cls, _name, signature().c_str());
		if constexpr (std::is_same<typename jni_traits<R>::jni_type, jobject>::value)
		{
			return	jobj<R>(local_obj::make(caller<R(Args...)>{}(_env, _obj, mid, ((typename jni_traits<Args>::jni_type)_args)...)));
		}
		else if constexpr (std::is_same<typename jni_traits<R>::jni_type, void>::value)
		{
			caller<R(Args...)>{}(_env, _obj, mid, ((typename jni_traits<Args>::jni_type)_args)...);
			return;
		}
		else if constexpr (!std::is_same<typename jni_traits<R>::jni_type, jobject>::value)
		{
			return	caller<R(Args...)>{}(_env, _obj, mid, ((typename jni_traits<Args>::jni_type)_args)...);
		}
	}
};

AMTRS_JAVA_NAMESPACE_END
#endif
