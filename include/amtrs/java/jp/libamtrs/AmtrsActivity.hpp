/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__jp_libamtrs_AmtrsActivity__hpp
#define	__libamtrs__android__java_classes__jp_libamtrs_AmtrsActivity__hpp
#include "../../androidx/fragment/app/FragmentActivity.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::view {
struct	View;
}

namespace jp::libamtrs {

struct	AmtrsActivityResult;


AMTRS_JAVA_DEFINE_CLASS(AmtrsActivity, androidx::fragment::app::FragmentActivity)
{
	using	View	= android::view::View;

	AMTRS_JAVA_CLASS_SIGNATURE("jp/libamtrs/AmtrsActivity");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


/*
		AMTRS_JAVA_DEFINE_METHOD(openDocument
			, void(AmtrsActivityResult, java::lang::String)
		)


		AMTRS_JAVA_DEFINE_METHOD(addActivityResult
			, int(AmtrsActivityResult)
		)
*/
		AMTRS_JAVA_DEFINE_METHOD(addView
			, void(View _topView)
		)


		AMTRS_JAVA_DEFINE_METHOD(foreground
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(pushView
			, void(View _topView)
		)

		AMTRS_JAVA_DEFINE_METHOD(popView
			, void()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
