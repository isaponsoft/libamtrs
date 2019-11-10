#ifndef	__libamtrs__java__exception__hpp
#define	__libamtrs__java__exception__hpp
#include "def.hpp"
#include "ref.hpp"
#include "jni_traits.hpp"
AMTRS_JAVA_NAMESPACE_BEGIN

inline extern bool has_exception()
{
	return	java::get_jnienv()->ExceptionCheck();
}

inline extern void clear_exception()
{
	java::get_jnienv()->ExceptionClear();
}


AMTRS_JAVA_NAMESPACE_END
#endif
