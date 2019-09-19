/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__jp_libamtrs_AmtrsActivityResult__hpp
#define	__libamtrs__android__java_classes__jp_libamtrs_AmtrsActivityResult__hpp
#include <amtrs/java/java/lang.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::content {
struct	Intent;
}

namespace jp::libamtrs {

struct	AmtrsActivityResult;

AMTRS_JAVA_DEFINE_CLASS(AmtrsActivityResult, java::lang::Object)
{
	using	Intent	= android::content::Intent;

	AMTRS_JAVA_CLASS_SIGNATURE("jp/libamtrs/AmtrsActivityResult");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(makeNativeBridge
			, AmtrsActivityResult(jlong _nativeHandler)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(onActivityResult
			, void(int _requestCode, int _resultCode, Intent _data)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
