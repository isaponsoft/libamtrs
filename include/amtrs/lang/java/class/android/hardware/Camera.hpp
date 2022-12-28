/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__hardware__Camera__hpp
#define	__libamtrs__android__java_classes__android__hardware__Camera__hpp
#include <amtrs/java/def.hpp>
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::hardware {

struct	Bitmap;
struct	BitmapFactory;


AMTRS_JAVA_DEFINE_CLASS(Camera, classes::java::lang::Object)
{
	using	String	= classes::java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/hardware/Camera");


	AMTRS_JAVA_DEFINE_CLASS(CameraInfo, classes::java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/hardware/Camera$CameraInfo");

		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;

			AMTRS_JAVA_DEFINE_INIT(
				CameraInfo()
			)

			AMTRS_JAVA_DEFINE_FIELD(jint, CAMERA_FACING_BACK)
			AMTRS_JAVA_DEFINE_FIELD(jint, CAMERA_FACING_FRONT)
		};

		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;

			AMTRS_JAVA_DEFINE_FIELD(jboolean, canDisableShutterSound)
			AMTRS_JAVA_DEFINE_FIELD(jint, facing)
			AMTRS_JAVA_DEFINE_FIELD(jint, orientation)
		};
	};


	AMTRS_JAVA_DEFINE_CLASS(Parameters, classes::java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/hardware/Camera$Parameters");

		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;
		};

		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;


			AMTRS_JAVA_DEFINE_METHOD(
				getPictureFormat,
				jint()
			)

			AMTRS_JAVA_DEFINE_METHOD(
				getPictureSize,
				Size()
			)

			AMTRS_JAVA_DEFINE_METHOD(
				getPreviewFormat,
				jint()
			)

			AMTRS_JAVA_DEFINE_METHOD(
				getPreviewSize,
				Size()
			)
		};
	};


	AMTRS_JAVA_DEFINE_CLASS(Size, classes::java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/hardware/Camera$Size");

		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;

			AMTRS_JAVA_DEFINE_INIT(
				Size(int w, int h)
			)
		};

		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;

			AMTRS_JAVA_DEFINE_FIELD(jint, height)
			AMTRS_JAVA_DEFINE_FIELD(jint, width)
		};
	};

	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_NEW_PICTURE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_NEW_VIDEO)
		AMTRS_JAVA_DEFINE_FIELD(jint, CAMERA_ERROR_EVICTED)
		AMTRS_JAVA_DEFINE_FIELD(jint, CAMERA_ERROR_SERVER_DIED)
		AMTRS_JAVA_DEFINE_FIELD(jint, CAMERA_ERROR_UNKNOWN)


		AMTRS_JAVA_DEFINE_METHOD(
			getCameraInfo,
			void(jint cameraId, CameraInfo cameraInfo)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			getNumberOfCameras,
			jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(
			open,
			Camera(),
			Camera(jint cameraId)
		)

	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(
			getParameters,
			Parameters()
		)

		AMTRS_JAVA_DEFINE_METHOD(
			startPreview,
			void()
		)

		AMTRS_JAVA_DEFINE_METHOD(
			stopPreview,
			void()
		)
	};

};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
