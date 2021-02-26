/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__jp_libamtrs_window__GLSurfaceWindow__hpp
#define	__libamtrs__android__java_classes__jp_libamtrs_window__GLSurfaceWindow__hpp
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::content {
struct	Context ;
}
namespace android::opengl {
struct	EGLConfig ;
}

namespace jp::libamtrs::window {


AMTRS_JAVA_DEFINE_CLASS(GLSurfaceWindow, java::lang::Object)
{
	using	Context		= android::content::Context;
	using	EGLConfig	= android::opengl::EGLConfig;

	AMTRS_JAVA_CLASS_SIGNATURE("jp/libamtrs/window/GLSurfaceWindow");


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

		AMTRS_JAVA_DEFINE_METHOD(onDrawFrame
			, void(long _nativeHandle)
		)

		AMTRS_JAVA_DEFINE_METHOD(onSurfaceChanged
			, void(long _nativeHandle, int width, int height)
		)

		AMTRS_JAVA_DEFINE_METHOD(onSurfaceCreated
			, void(long _nativeHandle, EGLConfig config)
		)

		AMTRS_JAVA_DEFINE_METHOD(onPause
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(onResume
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(requestRender
			, void()
		)

	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
