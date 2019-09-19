#ifndef	__libamtrs__java__new_object__hpp
#define	__libamtrs__java__new_object__hpp
#include "def.hpp"
#include "ref.hpp"
#include "jni_traits.hpp"
AMTRS_JAVA_NAMESPACE_BEGIN




AMTRS_IMPLEMENTS_BEGIN(new_object)
template<class...>
struct	new_object_impl;

template<class R, class... Args>
struct	new_object_impl<R(Args...)>
{
	jobject operator () (JNIEnv* _env, jclass _cls, jmethodID _mid, Args&&... _args)
	{
		return	_env->NewObject(_cls, _mid, std::forward<Args>(_args)...);
	}
};
AMTRS_IMPLEMENTS_END(new_object)



template<class...>
struct	new_object;

template<class... Args>
struct	new_object<void(Args...)>
{
	jobject operator () (JNIEnv* _env, jclass _cls, jmethodID _mid, typename jni_traits<Args>::jni_type... _args)
	{
		return	_env->NewObject(_cls, _mid, _args...);
	}
};





AMTRS_JAVA_NAMESPACE_END
#endif
