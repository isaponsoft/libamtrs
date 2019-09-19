/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__os__android__android_app__hpp
#define	__libamtrs__os__android__android_app__hpp

#include <android_native_app_glue.h>
#include "def.hpp"
#include "../../logging.hpp"
#include "../../java.hpp"
#include "../../java/android/app/Activity.hpp"
#include "../../java/jp/libamtrs/AmtrsActivity.hpp"


AMTRS_OS_ANDROID_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(get_android_app)
struct	app_instance
{
	static inline java::globalref<jobject>& main_activity()
	{
		static java::globalref<jobject>	s_object;
		return	s_object;
	}
};

AMTRS_IMPLEMENTS_END(get_android_app)



// ============================================================================
//! Java Bridge を使って Activity を取得します。
// ----------------------------------------------------------------------------
inline java::jobj<amtrs::java::classes::jp::libamtrs::AmtrsActivity> get_activity()
{
	auto&		obj	= AMTRS_IMPLEMENTS(get_android_app)::app_instance::main_activity();
	return	java::jobj<amtrs::java::classes::jp::libamtrs::AmtrsActivity>(java::local_obj(obj.get()));
}


AMTRS_OS_ANDROID_NAMESPACE_END
#endif
