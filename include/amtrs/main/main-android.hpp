/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__main__main_android__hpp
#define	__libamtrs__main__main_android__hpp
#include "../os/android/android_app.hpp"
#include "../os/android/amtrs_activity.hpp"



AMTRS_NAMESPACE_BEGIN
struct	activity_proxy
{
	static jlong initialize_activity(jobject _activity);
};
AMTRS_NAMESPACE_END



#define	AMTRS_GUI_MAIN(_mainclass)	\
	AMTRS_NAMESPACE_BEGIN\
	jlong activity_proxy::initialize_activity(jobject _activity)\
	{\
		amtrs::os::android::amtrs_activity::set_application(new _mainclass());\
		return amtrs::os::android::amtrs_activity::initialize(_activity);\
	};\
	AMTRS_NAMESPACE_END


extern "C" JNIEXPORT jlong JNICALL Java_jp_libamtrs_ActivityInitializer_initialize(JNIEnv* _env, jclass _clz, jobject _activity)
{
	amtrs::java::set_jnienv(_env);
	return	amtrs::activity_proxy::initialize_activity(_activity);
}


#endif
