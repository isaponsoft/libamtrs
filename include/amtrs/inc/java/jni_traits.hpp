#ifndef	__libamtrs__java__jni_traits__hpp
#define	__libamtrs__java__jni_traits__hpp
#include <jni.h>
#include "../../string.hpp"
#include "def.hpp"

AMTRS_JAVA_NAMESPACE_BEGIN




template<class T>
struct	jni_traits<T>
{
	using					jni_type	= jobject;
	using					argv_type	= const jobj<T>&;
	static constexpr auto	classname() noexcept { return T::classname(); }
	static constexpr auto	signature() noexcept { return T::signature(); }
};



template<class T>
struct	jni_traits<T[]>
{
	using					jni_type	= typename jni_traits<T>::jni_type*;
	using					argv_type	= jni_type;
	static constexpr auto	signature() noexcept { return make_constring("[") + jni_traits<T>::signature(); }
};


template<class T>
struct	jni_traits<T*>
{
	using					jni_type	= typename jni_traits<T>::jni_type*;
	using					argv_type	= jni_type;
	static constexpr auto	signature() noexcept { return make_constring("[") + jni_traits<T>::signature(); }
};

template<>
struct	jni_traits<jbyteArray>
{
	using					jni_type	= jbyteArray;
	using					argv_type	= jni_type;
	static constexpr auto	signature() noexcept { return make_constring("[B"); }
};

template<>
struct	jni_traits<jintArray>
{
	using					jni_type	= jintArray;
	using					argv_type	= jni_type;
	static constexpr auto	signature() noexcept { return make_constring("[I"); }
};

template<>
struct	jni_traits<jfloatArray>
{
	using					jni_type	= jfloatArray;
	using					argv_type	= jni_type;
	static constexpr auto	signature() noexcept { return make_constring("[F"); }
};

template<>
struct	jni_traits<int[]> : jni_traits<jintArray>
{
};



template<>
struct	jni_traits<void>
{
	using					jni_type	= void;
	using					argv_type	= jni_type;
	static constexpr auto	signature() noexcept { return make_constring("V"); }
};


template<>
struct	jni_traits<jboolean>
{
	using					jni_type	= jboolean;
	using					argv_type	= jni_type;
	static constexpr auto	signature() noexcept { return make_constring("Z"); }
};


template<>
struct	jni_traits<jbyte>
{
	using					jni_type	= jbyte;
	using					argv_type	= jni_type;
	static constexpr auto	signature() noexcept { return make_constring("B"); }
};



template<>
struct	jni_traits<jchar>
{
	using					jni_type	= jchar;
	using					argv_type	= jni_type;
	static constexpr auto	signature() noexcept { return make_constring("C"); }
};



template<>
struct	jni_traits<jshort>
{
	using					jni_type	= jshort;
	using					argv_type	= jni_type;
	static constexpr auto	signature() noexcept { return make_constring("S"); }
};


template<>
struct	jni_traits<jint>
{
	using					jni_type	= jint;
	using					argv_type	= jni_type;
	static constexpr auto	signature() noexcept { return make_constring("I"); }
};


template<>
struct	jni_traits<jlong>
{
	using					jni_type	= jlong;
	using					argv_type	= jni_type;
	static constexpr auto	signature() noexcept { return make_constring("J"); }
};


template<>
struct	jni_traits<jfloat>
{
	using					jni_type	= jfloat;
	using					argv_type	= jni_type;
	static constexpr auto	signature() noexcept { return make_constring("F"); }
};


template<>
struct	jni_traits<jdouble>
{
	using					jni_type	= jdouble;
	using					argv_type	= jni_type;
	static constexpr auto	signature() noexcept { return make_constring("D"); }
};


template<>
struct	jni_traits<jobject>
{
	using					jni_type	= jobject;
	using					argv_type	= jni_type;
	static constexpr auto	classname() noexcept { return make_constring("java/lang/Object"); }
	static constexpr auto	signature() noexcept { return make_constring("L") + classname() + make_constring(";"); }
};


template<>
struct	jni_traits<jclass>
{
	using					jni_type	= jclass;
	using					argv_type	= jni_type;
	static constexpr auto	classname() noexcept { return make_constring("java/lang/Class"); }
	static constexpr auto	signature() noexcept { return make_constring("L") + classname() + make_constring(";"); }
};



template<>
struct	jni_traits<jstring>
{
	using					jni_type	= jstring;
	using					argv_type	= jni_type;
	static constexpr auto	classname() noexcept { return make_constring("java/lang/String"); }
	static constexpr auto	signature() noexcept { return make_constring("L") + classname() + make_constring(";"); }
};



template<class T>
struct	jni_traits<jobj<T>>
{
	using					jni_type	= jobject;
	using					argv_type	= const jobj<T>&;
	static constexpr auto	classname() noexcept { return jobj<T>::classname(); }
	static constexpr auto	signature() noexcept { return jobj<T>::signature(); }
};


template<class Traits, class Allocator>
struct	jni_traits<std::basic_string<char, Traits, Allocator>>
		: jni_traits<jstring>
{
};


template<class First, class... Params>
struct	jni_traits<First, Params...>
{
	static constexpr auto	signature() noexcept
	{
		using	type	= typename std::conditional<std::is_base_of<jni_traits<jobject>, First>::value, First, jni_traits<First>>::type;
		return	type::signature() + jni_traits<Params...>::signature();
	}
};


template<class Result>
struct	jni_traits<Result()>
{
	using	jni_type		= typename jni_traits<Result>::jni_type;
	using	result_type		= Result;

	static constexpr auto	signature() noexcept { return make_constring("()") + jni_traits<result_type>::signature(); }
};


template<class Result, class... Params>
struct	jni_traits<Result(Params...)>
{
	using	jni_type		= typename jni_traits<Result>::jni_type;
	using	result_type		= Result;

	static constexpr auto	signature() noexcept
	{
		return	make_constring("(")
				+ jni_traits<Params...>::signature()
				+ make_constring(")")
				+ jni_traits<result_type>::signature();
	}
};

template<class T>
constexpr auto make_signature() noexcept
{
	return	jni_traits<T>::signature();
}



AMTRS_JAVA_NAMESPACE_END
#endif
