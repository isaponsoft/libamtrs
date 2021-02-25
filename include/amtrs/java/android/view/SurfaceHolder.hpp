/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android_view__SurfaceHolder__hpp
#define	__libamtrs__android__java_classes__android_view__SurfaceHolder__hpp
#include "../../java/lang/Object.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::graphics {
struct	Canvas;
struct	Rect;
}

namespace android::view {

struct	Surface;


AMTRS_JAVA_DEFINE_CLASS(SurfaceHolder, java::lang::Object)
{
	using	Canvas	= android::graphics::Canvas;
	using	Rect	= android::graphics::Rect;


	AMTRS_JAVA_CLASS_SIGNATURE("android/view/SurfaceHolder");


	AMTRS_JAVA_DEFINE_CLASS(Callback, java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/view/SurfaceHolder$Callback");

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


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(jint, SURFACE_TYPE_GPU)
		AMTRS_JAVA_DEFINE_FIELD(jint, SURFACE_TYPE_HARDWARE)
		AMTRS_JAVA_DEFINE_FIELD(jint, SURFACE_TYPE_NORMAL)
		AMTRS_JAVA_DEFINE_FIELD(jint, SURFACE_TYPE_PUSH_BUFFERS)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(addCallback
			, void(Callback callback)
		)

		AMTRS_JAVA_DEFINE_METHOD(getSurface
			, Surface()
		)

		AMTRS_JAVA_DEFINE_METHOD(getSurfaceFrame
			, Rect()
		)

		AMTRS_JAVA_DEFINE_METHOD(isCreating
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(lockCanvas
			, Canvas()
			, Canvas(Rect dirty)
		)

		AMTRS_JAVA_DEFINE_METHOD(lockHardwareCanvas
			, Canvas()
		)

		AMTRS_JAVA_DEFINE_METHOD(removeCallback
			, void(Callback callback)
		)

		AMTRS_JAVA_DEFINE_METHOD(setFixedSize
			, void(jint width, jint height)
		)

		AMTRS_JAVA_DEFINE_METHOD(setFormat
			, void(jint format)
		)

		AMTRS_JAVA_DEFINE_METHOD(setKeepScreenOn
			, void(jboolean screenOn)
		)

		AMTRS_JAVA_DEFINE_METHOD(setSizeFromLayout
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(setType
			, void(jint type)
		)

		AMTRS_JAVA_DEFINE_METHOD(unlockCanvasAndPost
			, void(Canvas canvas)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
