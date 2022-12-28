/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__java__java__lang__Number__hpp
#define	__libamtrs__java__java__lang__Number__hpp
#include "Object.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::lang {

AMTRS_JAVA_DEFINE_CLASS(Number, Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("java/lang/Number");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(
			byteValue,
			jbyte()
		)

		AMTRS_JAVA_DEFINE_METHOD(
			doubleValue,
			jdouble()
		)

		AMTRS_JAVA_DEFINE_METHOD(
			floatValue,
			jfloat()
		)

		AMTRS_JAVA_DEFINE_METHOD(
			intValue,
			jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(
			longValue,
			jlong()
		)

		AMTRS_JAVA_DEFINE_METHOD(
			shortValue,
			jshort()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
