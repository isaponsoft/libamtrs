/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_util_DisplayMetrics__hpp
#define	__libamtrs__android__java_classes__android_util_DisplayMetrics__hpp
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::util {

// https://developer.android.com/reference/android/content/res/Resources.html
AMTRS_JAVA_DEFINE_CLASS(DisplayMetrics, java::lang::Object)
{
	using	String				= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/util/DisplayMetrics");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(int, DENSITY_DEVICE_STABLE)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(float, density)
		AMTRS_JAVA_DEFINE_FIELD(int, densityDpi)
		AMTRS_JAVA_DEFINE_FIELD(int, heightPixels)
		AMTRS_JAVA_DEFINE_FIELD(float, scaledDensity)
		AMTRS_JAVA_DEFINE_FIELD(int, widthPixels)
		AMTRS_JAVA_DEFINE_FIELD(float, xdpi)
		AMTRS_JAVA_DEFINE_FIELD(float, ydpi)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
