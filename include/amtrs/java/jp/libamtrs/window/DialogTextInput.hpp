/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__jp__libamtrs__window__DialogTextInput__hpp
#define	__libamtrs__android__java_classes__jp__libamtrs__window__DialogTextInput__hpp
#include <amtrs/java/android/widget/EditText.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::content {
struct	Context;
}


namespace jp::libamtrs::window {

AMTRS_JAVA_DEFINE_CLASS(DialogTextInput, android::widget::EditText)
{
	using	Context	= android::content::Context;

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

		AMTRS_JAVA_DEFINE_METHOD(close
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(imeShow
			, void(jboolean _visible)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
