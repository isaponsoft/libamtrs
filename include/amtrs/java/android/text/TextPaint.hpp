/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android__text_TextPaint__hpp
#define	__libamtrs__android__java_classes__android__text_TextPaint__hpp
#include <amtrs/java/def.hpp>

#include <amtrs/java/android/graphics/Paint.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::text {



AMTRS_JAVA_DEFINE_CLASS(TextPaint, android::graphics::Paint)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/text/TextPaint");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_INIT(
			  void()
			, void(jint flags)
			, void(Paint p)
		)
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
