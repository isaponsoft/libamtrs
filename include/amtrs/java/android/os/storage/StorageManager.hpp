/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_os_storage_StorageManager__hpp
#define	__libamtrs__android__java_classes__android_os_storage_StorageManager__hpp
#include <amtrs/java/def.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN


namespace android::os::storage {

struct	StorageVolume;

// https://developer.android.com/reference/android/os/storage/StorageManager
AMTRS_JAVA_DEFINE_CLASS(StorageManager, java::lang::Object)
{
	using	String				= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/os/storage/StorageManager");


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


		AMTRS_JAVA_DEFINE_METHOD(getPrimaryStorageVolume
			, StorageVolume()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
