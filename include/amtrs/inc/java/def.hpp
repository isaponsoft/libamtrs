#ifndef	__libamtrs__inc__java__def__hpp
#define	__libamtrs__inc__java__def__hpp
// Java(TM) support utility.
#include <jni.h>
#include "../../amtrs.hpp"
#define	AMTRS_JAVA_NAMESPACE_BEGIN	AMTRS_NAMESPACE_BEGIN namespace java {
#define	AMTRS_JAVA_NAMESPACE_END	} AMTRS_NAMESPACE_END
AMTRS_JAVA_NAMESPACE_BEGIN


template<class T>		struct	jcls;		// Prototype, not use.
template<class T>		struct	jobj;		// Prototype, not use.
template<class...>		struct	jni_traits;
template<class JniT>	class	lref;
template<class JniT>	class	gref;
template<class T>		struct	argv;


template<class T>
struct	argv
{
	constexpr argv(T _o) : obj(_o) {}
	constexpr operator T () const noexcept { return obj; }
	T	obj;
};


AMTRS_JAVA_NAMESPACE_END
#endif
