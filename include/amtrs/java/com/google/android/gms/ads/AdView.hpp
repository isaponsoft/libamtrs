/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__com__google__android__gms__ads__AdView__hpp
#define	__libamtrs__android__java_classes__com__google__android__gms__ads__AdView__hpp
#include "../../../../../java/lang/String.hpp"
#include "../../../../../android/content/Context.hpp"
#include "../../../../../android/view/View.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::util {
struct	Date;
}
namespace android::location {
struct	Location;
}

namespace com::google::android::gms::ads {

using namespace java::lang;

struct	AdListener;
struct	AdRequest;
struct	AdSize;


// https://developers.google.com/android/reference/com/google/android/gms/ads/AdView
AMTRS_JAVA_DEFINE_CLASS(AdView, amtrs::java::classes::android::view::View)
{
	using	Context		= amtrs::java::classes::android::content::Context;

	AMTRS_JAVA_CLASS_SIGNATURE("com/google/android/gms/ads/AdView");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_INIT(void(Context))
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(destroy
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(getAdListener
			, AdListener()
		)

		AMTRS_JAVA_DEFINE_METHOD(getAdSize
			, AdSize()
		)

		AMTRS_JAVA_DEFINE_METHOD(getAdUnitId
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getMediationAdapterClassName
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(isLoading
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(loadAd
			, void(AdRequest adRequest)
		)

		AMTRS_JAVA_DEFINE_METHOD(pause
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(resume
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(setAdListener
			, void(AdListener adListener)
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
