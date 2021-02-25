/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__accounts__AccountManager__hpp
#define	__libamtrs__android__java_classes__android__accounts__AccountManager__hpp
#include "../../java/lang/String.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::content {
struct	Context;
}

namespace android::accounts {

AMTRS_JAVA_DEFINE_CLASS(AccountManager, java::lang::Object)
{
	using	Context			= android::content::Context;


	AMTRS_JAVA_CLASS_SIGNATURE("android/accounts/AccountManager");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(get
			, AccountManager(Context context)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getAccounts
			, Account*()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
