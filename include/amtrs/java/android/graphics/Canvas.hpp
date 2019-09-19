/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android__graphics__Canvas__hpp
#define	__libamtrs__android__java_classes__android__graphics__Canvas__hpp
#include <amtrs/java/def.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::graphics {

struct	Bitmap;
struct	Paint;

AMTRS_JAVA_DEFINE_CLASS(Canvas, java::lang::Object)
{
	using	Bitmap	= android::graphics::Bitmap;
	using	Paint	= android::graphics::Paint;
	using	String	= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/graphics/Canvas");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_INIT(
			  void()
			, void(Bitmap)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(
			drawText,
			void(String, jfloat x, jfloat y, Paint paint)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
