/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__graphics__Bitmap__hpp
#define	__libamtrs__android__java_classes__android__graphics__Bitmap__hpp
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::graphics {

struct	Bitmap;
struct	BitmapFactory;


AMTRS_JAVA_DEFINE_CLASS(Bitmap, classes::java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/graphics/Bitmap");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;


		//! Bitmap createBitmap(width, height, Config)
		AMTRS_JAVA_DEFINE_METHOD(
			createBitmap,
			Bitmap(jint _width, jint _height, Config _config)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(
			getWidth,
			jint()
		)
	};


	AMTRS_JAVA_DEFINE_CLASS(Config, classes::java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/graphics/Bitmap$Config");

		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;

			AMTRS_JAVA_DEFINE_FIELD(Config, ARGB_8888)
		};

		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;
		};
	};
};



}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
