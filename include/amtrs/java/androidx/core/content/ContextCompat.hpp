/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__core__content__ContextCompat__hpp
#define	__libamtrs__android__java_classes__android__core__content__ContextCompat__hpp
#include "../../../java/lang/Object.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace java::lang {
struct	String;
}

namespace android::content {
struct	Context;
}

namespace androidx::core::content {


AMTRS_JAVA_DEFINE_CLASS(ContextCompat , java::lang::Object)
{
	using	Context		= android::content::Context;
	using	String		= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("androidx/core/content/ContextCompat");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(checkSelfPermission
				, jint(Context context, String permission)
			)

		AMTRS_JAVA_DEFINE_METHOD(createDeviceProtectedStorageContext
				, Context(Context context)
			)

		AMTRS_JAVA_DEFINE_METHOD(getColor
				, jint(Context context, jint id)
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
