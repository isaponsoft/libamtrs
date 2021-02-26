/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_view__SurfaceView__hpp
#define	__libamtrs__android__java_classes__android_view__SurfaceView__hpp
#include "View.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::graphics {
struct	Canvas;
struct	Region;
}

namespace android::view {

struct	SurfaceHolder;

AMTRS_JAVA_DEFINE_CLASS(SurfaceView, View)
{
	using	Canvas	= android::graphics::Canvas;
	using	Region	= android::graphics::Region;

	AMTRS_JAVA_CLASS_SIGNATURE("android/view/SurfaceView");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(draw
			, void(Canvas canvas)
		)

		AMTRS_JAVA_DEFINE_METHOD(gatherTransparentRegion
			, jboolean(Region region)
		)

		AMTRS_JAVA_DEFINE_METHOD(getHolder
			, SurfaceHolder()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
