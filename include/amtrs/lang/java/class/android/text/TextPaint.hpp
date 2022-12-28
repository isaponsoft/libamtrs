/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__text_TextPaint__hpp
#define	__libamtrs__android__java_classes__android__text_TextPaint__hpp
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
