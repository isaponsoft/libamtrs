/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android__graphics__Rect__hpp
#define	__libamtrs__android__java_classes__android__graphics__Rect__hpp
#include <amtrs/java/def.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::graphics {


AMTRS_JAVA_DEFINE_CLASS(Rect, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/graphics/Rect");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_INIT(
			  void()
			, void(jint left, jint top, jint right, jint bottom)
			, void(Rect)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(int, bottom)
		AMTRS_JAVA_DEFINE_FIELD(int, left)
		AMTRS_JAVA_DEFINE_FIELD(int, right)
		AMTRS_JAVA_DEFINE_FIELD(int, top)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
