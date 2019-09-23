/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_os_Environment__hpp
#define	__libamtrs__android__java_classes__android_os_Environment__hpp
#include <amtrs/java/def.hpp>

#include <amtrs/java/android/os/Environment.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace java::io {
	struct	File;
}

namespace android::os {

// https://developer.android.com/reference/android/os/Environment
AMTRS_JAVA_DEFINE_CLASS(Environment, java::lang::Object)
{
	using	File				= java::io::File;
	using	String				= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/os/Environment");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(String, DIRECTORY_ALARMS)
		AMTRS_JAVA_DEFINE_FIELD(String, DIRECTORY_DCIM)
		AMTRS_JAVA_DEFINE_FIELD(String, DIRECTORY_DOCUMENTS)

		AMTRS_JAVA_DEFINE_METHOD(getExternalStorageDirectory
			, File()
		)

		AMTRS_JAVA_DEFINE_METHOD(getExternalStoragePublicDirectory
			, File(String type)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;
	};
};



}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
