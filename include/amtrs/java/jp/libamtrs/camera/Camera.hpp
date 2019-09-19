/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__jp__libamtrs__camera__Camera__hpp
#define	__libamtrs__android__java_classes__jp__libamtrs__camera__Camera__hpp
#include <amtrs/java/android/hardware/Camera.hpp>
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::content {
struct	Context;
}


namespace jp::libamtrs::camera {


AMTRS_JAVA_DEFINE_CLASS(Camera, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("jp/libamtrs/camera/Camera");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_INIT(
			  Camera(jlong, android::content::Context, android::hardware::Camera)
			, Camera(jlong, android::content::Context, android::hardware::Camera, jint width, jint height)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(updateTime
			, void(jint _hourOfDay, jint _minuteOfHour)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
