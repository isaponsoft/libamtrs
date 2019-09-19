/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_view_Window__hpp
#define	__libamtrs__android__java_classes__android_view_Window__hpp
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::view {

// https://developer.android.com/reference/android/view/Window
AMTRS_JAVA_DEFINE_CLASS(Window, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/view/Window");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(addFlags
			, void(int flag)
		)


		AMTRS_JAVA_DEFINE_METHOD(clearFlags
			, void(int flag)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
