/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android__graphics__Bitmap__hpp
#define	__libamtrs__android__java_classes__android__graphics__Bitmap__hpp
#include <amtrs/java/def.hpp>

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



//! https://developer.android.com/reference/android/graphics/BitmapFactory.html
AMTRS_JAVA_DEFINE_CLASS(BitmapFactory, classes::java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/graphics/BitmapFactory");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(
			decodeByteArray,
			Bitmap(jbyteArray _data, jint offset, jint length)
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
