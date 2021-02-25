/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_view_ViewGroup__hpp
#define	__libamtrs__android__java_classes__android_view_ViewGroup__hpp
#include "View.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::view {

// https://developer.android.com/reference/android/app/Activity
AMTRS_JAVA_DEFINE_CLASS(ViewGroup, View)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/view/ViewGroup");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(jint	, CLIP_TO_PADDING_MASK)
		AMTRS_JAVA_DEFINE_FIELD(jint	, FOCUS_AFTER_DESCENDANTS)
		AMTRS_JAVA_DEFINE_FIELD(jint	, FOCUS_BEFORE_DESCENDANTS)
		AMTRS_JAVA_DEFINE_FIELD(jint	, FOCUS_BLOCK_DESCENDANTS)
		AMTRS_JAVA_DEFINE_FIELD(jint	, LAYOUT_MODE_CLIP_BOUNDS)
		AMTRS_JAVA_DEFINE_FIELD(jint	, LAYOUT_MODE_OPTICAL_BOUNDS)
		AMTRS_JAVA_DEFINE_FIELD(jint	, PERSISTENT_ALL_CACHES)
		AMTRS_JAVA_DEFINE_FIELD(jint	, PERSISTENT_ANIMATION_CACHE)
		AMTRS_JAVA_DEFINE_FIELD(jint	, PERSISTENT_NO_CACHE)
		AMTRS_JAVA_DEFINE_FIELD(jint	, PERSISTENT_SCROLLING_CACHE)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;
	};

	AMTRS_JAVA_DEFINE_CLASS(LayoutParams, _ViewGrpup_LayoutParams)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/view/ViewGroup$LayoutParams");

		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;

			//AMTRS_JAVA_DEFINE_FIELD(jint	, FILL_PARENT)
			AMTRS_JAVA_DEFINE_FIELD(jint	, MATCH_PARENT)
			AMTRS_JAVA_DEFINE_FIELD(jint	, WRAP_CONTENT)
		};

		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;
		};
	};


	AMTRS_JAVA_DEFINE_CLASS(MarginLayoutParams, LayoutParams)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/view/ViewGroup$MarginLayoutParams");

		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;
		};

		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;
		};
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
