/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__java__native_method__hpp
#define	__libamtrs__java__native_method__hpp
AMTRS_JAVA_NAMESPACE_BEGIN




// ****************************************************************************
//! JNIのネイティブメソッドのブリッジを簡単に定義するためのヘルパです。
// ----------------------------------------------------------------------------
//!	example)
//!		class	MyClass
//!		{
//!			// 第１引数は必ず long 型にして、ネイティブクラスのポインタを
//!			// 渡すようにします。
//!			public native boolean nativ_emethod(long cppThis, int p1, MyClass2 p2);
//!		
//!
//!		class	cpp_bride_class
//!		{
//!		private:
//!			jboolean nativ_emethod1(jint p1, jobj<java::classes::MyClass2> param2) {
//!			int nativ_emethod2() {
//!
//!			void initialize()
//!			{
//!				AMTRS_JAVA_JNIMETHOD_REGISTER(java::classes::MyClass, cpp_bride_class,
//!					AMTRS_JAVA_MAKE_JNIMETHOD(nativ_emethod1),
//!					AMTRS_JAVA_MAKE_JNIMETHOD(nativ_emethod2)
//!				);
// ----------------------------------------------------------------------------
#define	AMTRS_JAVA_JNIMETHOD_REGISTER(_javaclass, _cppclass, ...)\
		using	nativeclass_type	= _cppclass;\
		static	bool	s_initialized	= false;\
		if (!s_initialized)\
		{\
			s_initialized	= true;\
			static constexpr auto	descriptor	= std::make_tuple(__VA_ARGS__);\
			static constexpr auto	tuple_size	= std::tuple_size<decltype(descriptor)>::value;\
			amtrs::java::jnimathod_table<_javaclass>(descriptor, std::make_index_sequence<tuple_size>{});\
		}


// ****************************************************************************
//! JNIのネイティブメソッドのブリッジを簡単に定義するためのヘルパです。
// ----------------------------------------------------------------------------
//!	AMTRS_JAVA_MAKE_JNIFUNCTION()の代わりに使用できます。
//!	クラス名を事前に using nativeclass_type = で設定しておく必要があります。
// ----------------------------------------------------------------------------
#define	AMTRS_JAVA_MAKE_JNIMETHOD(_name)\
		amtrs::java::jniwrapper<decltype(&nativeclass_type::_name)>(\
			#_name, \
			[](auto... _args){\
				return	amtrs::java::jniwrapper<decltype(&nativeclass_type::_name)>::invoke(&nativeclass_type::_name, _args...);\
			}\
		)
		



template<class...>
struct	jniwrapper;


template<class T, class R, class... Args>
struct	jniwrapper<R(T::*)(Args...)>
{
	using	bridge_type		= R(*)(JNIEnv*, jobject, jlong, typename java::jni_traits<Args>::jni_type...);
	using	function_type	= R(T::*)(Args...);
	using	signature_type	= decltype(java::make_signature<R(jlong, Args...)>());

	constexpr jniwrapper(const char* _name, bridge_type _bridge)
		: signature(java::make_signature<R(jlong, Args...)>())
		, name(_name)
		, bridge(_bridge)
	{}

	operator JNINativeMethod () const noexcept { return {name, signature.c_str(), (void*)bridge}; }

	static constexpr R invoke(R(T::* _method)(Args...), JNIEnv* _env, jobject _thiz, jlong _handle, typename java::jni_traits<Args>::jni_type... _args)
	{
		java::set_jnienv(_env);
		return	(reinterpret_cast<T*>(_handle)->*_method)(((Args)_args)...);
	}

	signature_type	signature;
	const char*		name;
	bridge_type		bridge;
};




template<class JavaC, class... Types, std::size_t... Is>
void jnimathod_table(std::tuple<Types...> tuple, std::index_sequence<Is...>)
{
	JNINativeMethod	methods[]	= { std::get<Is>(tuple)... };
	#ifdef	AMTRS_SYSTRACE_LOG_USE
	for (auto& m : methods)
	{
		AMTRS_SYSTRACE_LOG("%s : %s", m.name, m.signature);
	}
	#endif
	auto	cls	= JavaC::find();
	if (amtrs::java::get_jnienv()->RegisterNatives(cls.get(), methods, sizeof(methods)/sizeof(methods[0])) < 0)
	{}
}


AMTRS_JAVA_NAMESPACE_END
#endif
