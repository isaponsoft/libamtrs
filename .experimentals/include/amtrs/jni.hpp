/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__jni__hpp
#define	__libamtrs__jni__hpp
#include "amtrs.hpp"
#include "string.hpp"

#include <jni.h>
#define	AMTRS_JAVA_NAMESPACE_BEGIN	AMTRS_NAMESPACE_BEGIN namespace java {
#define	AMTRS_JAVA_NAMESPACE_END	} AMTRS_NAMESPACE_END
#define	AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN	AMTRS_JAVA_NAMESPACE_BEGIN namespace classes {
#define	AMTRS_JAVA_CLASSES_NAMESPACE_END	} AMTRS_JAVA_NAMESPACE_END
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
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
AMTRS_JAVA_CLASSES_NAMESPACE_END



// keep order.
#include ".inc/jni-jnienv.hpp"
#include ".inc/jni-ref.hpp"
#include ".inc/jni-jobj.hpp"
#include ".inc/jni-call_init.hpp"
#include ".inc/jni-call_method.hpp"
#include ".inc/jni-define_class.hpp"
#include ".inc/jni-exception.hpp"
#include ".inc/jni-jcls.hpp"
#include ".inc/jni-jni_traits.hpp"
#include ".inc/jni-native_method.hpp"
#include ".inc/jni-new_object.hpp"
#include ".inc/jni-string.hpp"
#endif
