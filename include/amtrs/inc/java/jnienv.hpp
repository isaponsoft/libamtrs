#ifndef	__libamtrs__java__jnienv__hpp
#define	__libamtrs__java__jnienv__hpp
#include <jni.h>
#include "def.hpp"
AMTRS_JAVA_NAMESPACE_BEGIN


// ============================================================================
//! JNIEnv* を取得します。
// ----------------------------------------------------------------------------
//! この関数はスレッドセーフです。また、事前にプラットフォームの指定する方法で
//! JNIEnv* を取得し、 set_jnienv() を呼び出しておく必要があります。
// ----------------------------------------------------------------------------
inline JNIEnv* get_jnienv();


// ============================================================================
//! JNIEnv* を設定します。
// ----------------------------------------------------------------------------
//! この関数はスレッドセーフです。
// ----------------------------------------------------------------------------
inline void set_jnienv(JNIEnv* _env);



AMTRS_IMPLEMENTS_BEGIN(jnienv)
template<class JNIENV>
struct	env
{
	static JNIENV*& value()
	{
		thread_local JNIENV*	s_env	= nullptr;
		return	s_env;
	}
};

AMTRS_IMPLEMENTS_END(jnienv)



inline JNIEnv* get_jnienv()
{
	return	AMTRS_IMPLEMENTS(jnienv)::env<JNIEnv>::value();
}



// ============================================================================
//! JNIEnv* を設定します。
// ----------------------------------------------------------------------------
//! この関数はスレッドセーフです。
// ----------------------------------------------------------------------------
inline void set_jnienv(JNIEnv* _env)
{
	AMTRS_IMPLEMENTS(jnienv)::env<JNIEnv>::value()	= _env;
}


AMTRS_JAVA_NAMESPACE_END
#endif
