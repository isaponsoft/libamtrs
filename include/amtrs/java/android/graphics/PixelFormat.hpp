/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__graphics__PixelFormat__hpp
#define	__libamtrs__android__java_classes__android__graphics__PixelFormat__hpp
#include "../../java/lang/Object.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::graphics {


AMTRS_JAVA_DEFINE_CLASS(PixelFormat, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/graphics/PixelFormat");


	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	
		AMTRS_JAVA_DEFINE_FIELD(jint, A_8)				// 0x00000008
		AMTRS_JAVA_DEFINE_FIELD(jint, JPEG)				// 0x00000100
		AMTRS_JAVA_DEFINE_FIELD(jint, LA_88)			// 0x0000000a
		AMTRS_JAVA_DEFINE_FIELD(jint, L_8)				// 0x00000009
		AMTRS_JAVA_DEFINE_FIELD(jint, OPAQUE)			// 0xffffffff
		AMTRS_JAVA_DEFINE_FIELD(jint, RGBA_1010102)		// 0x0000002b
		AMTRS_JAVA_DEFINE_FIELD(jint, RGBA_4444)		// 0x00000007
		AMTRS_JAVA_DEFINE_FIELD(jint, RGBA_5551)		// 0x00000006
		AMTRS_JAVA_DEFINE_FIELD(jint, RGBA_8888)		// 0x00000001
		AMTRS_JAVA_DEFINE_FIELD(jint, RGBA_F16)			// 0x00000016
		AMTRS_JAVA_DEFINE_FIELD(jint, RGBX_8888)		// 0x00000002
		AMTRS_JAVA_DEFINE_FIELD(jint, RGB_332)			// 0x0000000b
		AMTRS_JAVA_DEFINE_FIELD(jint, RGB_565)			// 0x00000004
		AMTRS_JAVA_DEFINE_FIELD(jint, RGB_888)			// 0x00000003
		AMTRS_JAVA_DEFINE_FIELD(jint, TRANSLUCENT)		// 0xfffffffd
		AMTRS_JAVA_DEFINE_FIELD(jint, TRANSPARENT)		// 0xfffffffe
		AMTRS_JAVA_DEFINE_FIELD(jint, UNKNOWN)			// 0x00000000
		AMTRS_JAVA_DEFINE_FIELD(jint, YCbCr_420_SP)		// 0x00000011
		AMTRS_JAVA_DEFINE_FIELD(jint, YCbCr_422_I)		// 0x00000014
		AMTRS_JAVA_DEFINE_FIELD(jint, YCbCr_422_SP)		// 0x00000010
	};


	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;
	};
	
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
