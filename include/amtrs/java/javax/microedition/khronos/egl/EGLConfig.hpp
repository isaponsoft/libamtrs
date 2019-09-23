/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__java__java_classes__javax_microedition_khronos_egl_EGLConfig__hpp
#define	__libamtrs__java__java_classes__javax_microedition_khronos_egl_EGLConfig__hpp
#include <amtrs/java/def.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace javax::microedition::khronos::egl {


AMTRS_JAVA_DEFINE_CLASS(EGLConfig, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("javax/microedition/khronos/egl/EGLConfig");


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
