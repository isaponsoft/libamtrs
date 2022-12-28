/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__com__google__android__gms__ads_AdSize__hpp
#define	__libamtrs__android__java_classes__com__google__android__gms__ads_AdSize__hpp
#include "../../../../../java/lang/String.hpp"
#include "../../../../../android/content/Context.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace com::google::android::gms::ads {


// https://developers.google.com/android/reference/com/google/android/gms/ads/AdSize
AMTRS_JAVA_DEFINE_CLASS(AdSize, java::lang::Object)
{
	using	Context	= amtrs::java::classes::android::content::Context;
	using	String	= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("com/google/android/gms/ads/AdSize");

	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(jint	, AUTO_HEIGHT)
		AMTRS_JAVA_DEFINE_FIELD(jint	, FULL_WIDTH)
		AMTRS_JAVA_DEFINE_FIELD(AdSize	, BANNER)
		AMTRS_JAVA_DEFINE_FIELD(AdSize	, FLUID)
		AMTRS_JAVA_DEFINE_FIELD(AdSize	, FULL_BANNER)
		AMTRS_JAVA_DEFINE_FIELD(AdSize	, INVALID)
		AMTRS_JAVA_DEFINE_FIELD(AdSize	, LARGE_BANNER)
		AMTRS_JAVA_DEFINE_FIELD(AdSize	, LEADERBOARD)
		AMTRS_JAVA_DEFINE_FIELD(AdSize	, MEDIUM_RECTANGLE)
		AMTRS_JAVA_DEFINE_FIELD(AdSize	, SEARCH)
		AMTRS_JAVA_DEFINE_FIELD(AdSize	, SMART_BANNER)
		AMTRS_JAVA_DEFINE_FIELD(AdSize	, WIDE_SKYSCRAPER)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(equals
			, jboolean(Object)
		)

		AMTRS_JAVA_DEFINE_METHOD(getHeight
			, int()
		)

		AMTRS_JAVA_DEFINE_METHOD(getHeightInPixels
			, int(Context context)
		)

		AMTRS_JAVA_DEFINE_METHOD(getWidth
			, int()
		)

		AMTRS_JAVA_DEFINE_METHOD(getWidthInPixels
			, int(Context context)
		)

		AMTRS_JAVA_DEFINE_METHOD(hashCode
			, int()
		)

		AMTRS_JAVA_DEFINE_METHOD(isAutoHeight
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(isFluid
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(isFullWidth
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(toString
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(setAdSize
			, void(AdSize adSize)
		)

		AMTRS_JAVA_DEFINE_METHOD(setAdUnitId
			, void(String adUnitId)
		)

	// Protected method.
		AMTRS_JAVA_DEFINE_METHOD(onLayout
			, void(jboolean changed, int left, int top, int right, int bottom)
		)

		AMTRS_JAVA_DEFINE_METHOD(onMeasure
			, void(int widthMeasureSpec, int heightMeasureSpec)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
