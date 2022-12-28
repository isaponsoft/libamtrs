/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__content__ContentUris__hpp
#define	__libamtrs__android__java_classes__android__content__ContentUris__hpp
#include "../net/Uri.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::content {


AMTRS_JAVA_DEFINE_CLASS(ContentUris , java::lang::Object)
{
	using	Uri		= android::net::Uri;


	AMTRS_JAVA_CLASS_SIGNATURE("android/content/ContentUris");

	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(
			appendId,
			Uri::Builder(Uri::Builder builder, jlong id)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			parseId,
			jlong(Uri contentUri)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			removeId,
			Uri(Uri contentUri)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			withAppendedId,
			Uri(Uri contentUri, jlong id)
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
