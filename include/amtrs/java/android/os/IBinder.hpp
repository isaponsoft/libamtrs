/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_os_IBinder__hpp
#define	__libamtrs__android__java_classes__android_os_IBinder__hpp
#include <amtrs/java/def.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::os {


AMTRS_JAVA_DEFINE_CLASS(IBinder, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/os/IBinder");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
