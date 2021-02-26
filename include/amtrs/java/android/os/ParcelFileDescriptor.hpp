/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__os__ParcelFileDescriptor__hpp
#define	__libamtrs__android__java_classes__android__os__ParcelFileDescriptor__hpp
#include "../../java/lang/String.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::io {
struct	FileDescriptor;
}
namespace android::os {


AMTRS_JAVA_DEFINE_CLASS(ParcelFileDescriptor, java::lang::Object)
{
	using	FileDescriptor	= java::io::FileDescriptor;
	using	String			= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/os/ParcelFileDescriptor");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(close
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(getFileDescriptor
			, FileDescriptor()
		)
	};
};




}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
