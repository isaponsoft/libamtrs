/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__androidx__fragment__app__DialogFragment__hpp
#define	__libamtrs__android__java_classes__androidx__fragment__app__DialogFragment__hpp
#include "Fragment.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace androidx::fragment::app {

struct	FragmentManager;
struct	FragmentTransaction;

AMTRS_JAVA_DEFINE_CLASS(DialogFragment, Fragment)
{
	using	String				= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("androidx/fragment/app/DialogFragment");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(show
			, void(FragmentManager manager, String tag)
//			, jint(FragmentTransaction transaction, String tag)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
