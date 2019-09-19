/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_os_storage_StorageVolume__hpp
#define	__libamtrs__android__java_classes__android_os_storage_StorageVolume__hpp
#include <amtrs/java/def.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN



namespace android::content {
	struct	Intent;
}


namespace android::os::storage {

// https://developer.android.com/reference/android/os/storage/StorageManager
AMTRS_JAVA_DEFINE_CLASS(StorageVolume, java::lang::Object)
{
	using	String				= java::lang::String;
	using	Intent				= android::content::Intent;

	AMTRS_JAVA_CLASS_SIGNATURE("android/os/storage/StorageVolume");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_MANAGE_STORAGE)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_REQUESTED_BYTES)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_UUID)


		AMTRS_JAVA_DEFINE_METHOD(createAccessIntent
			, Intent(String directoryName)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
