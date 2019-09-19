/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__application__application_android__hpp
#define	__libamtrs__application__application_android__hpp
#include "../../os/android/android_app.hpp"
#include "../../os/android/amtrs_activity.hpp"
#include "../../java/java/io/File.hpp"
#include "../../java/android/app/Activity.hpp"
#include "../../java/android/content/Context.hpp"
#include "../../java/android/content/pm/ApplicationInfo.hpp"
#include "../../java/android/content/Intent.hpp"
#include "../../java/android/os/Environment.hpp"
#include "../../java/android/os/storage/StorageManager.hpp"
#include "../../java/android/os/storage/StorageVolume.hpp"
#define	AMTRS_APPLICATION_ANDROID_DEBUG_LOG(...)	AMTRS_DEBUG_LOG(__VA_ARGS__)
AMTRS_NAMESPACE_BEGIN


inline void application::foreground()
{
	os::android::get_activity().foreground();
}


inline filesystem::path application::cache_dir() const
{
	using	Environment		= java::classes::android::os::Environment;
	auto	clsEnv		= Environment::find();
	auto	external	= clsEnv.getExternalStoragePublicDirectory(Environment::find().get_DIRECTORY_DOCUMENTS());
	auto	absPath		= external.getAbsolutePath();
	return	std::to_string((jstring)absPath.get());
}


inline filesystem::path application::documents_dir() const
{
	using	Environment		= java::classes::android::os::Environment;
	auto	clsEnv		= Environment::find();
	auto	external	= clsEnv.getExternalStoragePublicDirectory(Environment::find().get_DIRECTORY_DOCUMENTS());
	auto	absPath		= external.getAbsolutePath();
	return	std::to_string((jstring)absPath.get());
}


inline filesystem::path application::files_dir(bool _external) const
{
	auto	dir		= os::android::get_activity().getFilesDir().getPath();
	return	filesystem::path(std::to_string((jstring)dir.get()));
}



AMTRS_NAMESPACE_END
#endif
