/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__java__java__lang__Runtime__hpp
#define	__libamtrs__java__java__lang__Runtime__hpp
#include "Object.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::lang {


AMTRS_JAVA_DEFINE_CLASS(Runtime, Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("java/lang/Runtime");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getRuntime
			, Runtime()
		)

	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(freeMemory
			, jlong()
		)

		AMTRS_JAVA_DEFINE_METHOD(gc
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(maxMemory
			, jlong()
		)

		AMTRS_JAVA_DEFINE_METHOD(totalMemory
			, jlong()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
