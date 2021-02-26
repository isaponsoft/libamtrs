/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__graphics__BitmapFactory_hpp
#define	__libamtrs__android__java_classes__android__graphics__BitmapFactory_hpp
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::io {
struct	FileDescriptor;
}
namespace android::graphics {

struct	Bitmap;
struct	Rect;

//! https://developer.android.com/reference/android/graphics/BitmapFactory.html
AMTRS_JAVA_DEFINE_CLASS(BitmapFactory, classes::java::lang::Object)
{
	using	FileDescriptor	= java::io::FileDescriptor;

	AMTRS_JAVA_DEFINE_CLASS(Options, classes::java::lang::Object)
	{
		using	FileDescriptor	= java::io::FileDescriptor;

		AMTRS_JAVA_CLASS_SIGNATURE("android/graphics/BitmapFactory$Options");


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

	AMTRS_JAVA_CLASS_SIGNATURE("android/graphics/BitmapFactory");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(decodeByteArray
			, Bitmap(jbyteArray _data, jint offset, jint length)
		)

		AMTRS_JAVA_DEFINE_METHOD(decodeFileDescriptor
			, Bitmap(FileDescriptor fd)
			, Bitmap(FileDescriptor fd, Rect outPadding, Options opts)
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
