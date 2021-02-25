/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__jp_libamtrs_window__SurfaceView__hpp
#define	__libamtrs__android__java_classes__jp_libamtrs_window__SurfaceView__hpp
#include "../../../android/view/SurfaceView.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::content {
struct	Context;
}
namespace android::view {
struct	SurfaceHolder;
struct	SurfaceView;
}

namespace jp::libamtrs::window {


AMTRS_JAVA_DEFINE_CLASS(SurfaceView, android::view::SurfaceView)
{
	using	Context			= android::content::Context;
	using	SurfaceHolder	= android::view::SurfaceHolder;

	AMTRS_JAVA_CLASS_SIGNATURE("jp/libamtrs/window/SurfaceView");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;


		AMTRS_JAVA_DEFINE_INIT(
			  void(Context, jlong)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(surfaceChanged
			, void(long _nativeHandle, SurfaceHolder holder, int format, int width, int height)
		)

		AMTRS_JAVA_DEFINE_METHOD(surfaceCreated
			, void(long _nativeHandle, SurfaceHolder holder)
		)

		AMTRS_JAVA_DEFINE_METHOD(surfaceDestroyed
			, void(long _nativeHandle, SurfaceHolder holder)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
