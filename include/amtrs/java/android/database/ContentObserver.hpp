/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__database__ContentObserver__hpp
#define	__libamtrs__android__java_classes__android__database__ContentObserver__hpp
#include "../../java/lang/String.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::database {


AMTRS_JAVA_DEFINE_CLASS(ContentObserver, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/database/ContentObserver");


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


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
