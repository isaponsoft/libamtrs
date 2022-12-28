/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__java__jc__java__util_List__hpp
#define	__libamtrs__java__jc__java__util_List__hpp
#include "Collection.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::util {


AMTRS_JAVA_DEFINE_CLASS(List, Collection)
{

	AMTRS_JAVA_CLASS_SIGNATURE("java/util/List");

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
