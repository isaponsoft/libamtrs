/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__jp__libamtrs__window__DialogTextInput__hpp
#define	__libamtrs__android__java_classes__jp__libamtrs__window__DialogTextInput__hpp
#include "../../../android/widget/EditText.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::content {
struct	Context;
}


namespace jp::libamtrs::window {

AMTRS_JAVA_DEFINE_CLASS(DialogTextInput, android::view::View)
{
	using	Context	= android::content::Context;
	using	String	= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("jp/libamtrs/window/DialogTextInput");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;


		//! コンストラクタ
		AMTRS_JAVA_DEFINE_INIT(
			DialogTextInput(Context, jlong)
		)

	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(cancel
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(setText
			, void(String text)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
