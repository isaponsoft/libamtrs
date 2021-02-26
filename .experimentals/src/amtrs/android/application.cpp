/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/application.hpp>
#include <amtrs/.driver/android-api-android_app.hpp>
#include <amtrs/.driver/android-api-amtrs_activity.hpp>
#include <amtrs/java/java/io/File.hpp>
#include <amtrs/java/java/lang/String.hpp>
#include <amtrs/java/android/app/Activity.hpp>
#include <amtrs/java/android/content/Context.hpp>
#include <amtrs/java/android/content/pm/ApplicationInfo.hpp>
#include <amtrs/java/android/content/Intent.hpp>
#include <amtrs/java/android/os/Environment.hpp>
#include <amtrs/java/android/os/storage/StorageManager.hpp>
#include <amtrs/java/android/os/storage/StorageVolume.hpp>
#include <amtrs/java/android/view/Window.hpp>
#include <amtrs/java/android/view/WindowManager.hpp>
AMTRS_NAMESPACE_BEGIN


void application::foreground()
{
	os::android::get_activity().foreground();
}


std::string application::cache_dir() const
{
	if (mCacheDir.empty())
	{
		auto	external	= os::android::get_activity().getExternalFilesDir(java::jobj<java::classes::java::lang::String>());
		auto	absPath		= external.getAbsolutePath();
		mCacheDir	= std::to_string((jstring)absPath.get());
	}
	return	mCacheDir;
}


std::string application::documents_dir() const
{
	if (mDocumentsDir.empty())
	{
		using	Environment		= java::classes::android::os::Environment;
		auto	clsEnv		= Environment::find();
		auto	external	= os::android::get_activity().getExternalFilesDir(clsEnv.get_DIRECTORY_PICTURES());
		auto	absPath		= external.getAbsolutePath();
		mDocumentsDir	= std::to_string((jstring)absPath.get());
	}
	return	mDocumentsDir;
}


std::string application::files_dir(bool _external) const
{
	if (mFilesDir.empty())
	{
		auto	dir		= os::android::get_activity().getFilesDir().getPath();
		mFilesDir	= std::to_string((jstring)dir.get());
	}
	return	mFilesDir;
}


void application::set_sleep_enable(bool _enable)
{
	using	lp	= java::classes::android::view::WindowManager::LayoutParams;
	auto	cls		= lp::find();
	if (_enable)
	{
		int		flags	= cls.get_FLAG_KEEP_SCREEN_ON()
						| cls.get_FLAG_TURN_SCREEN_ON()
						| cls.get_FLAG_DISMISS_KEYGUARD()
		;
		os::android::get_activity().getWindow().clearFlags(flags);
	}
	else
	{
		int		flags	= cls.get_FLAG_KEEP_SCREEN_ON()
						| cls.get_FLAG_TURN_SCREEN_ON()
						| cls.get_FLAG_DISMISS_KEYGUARD()
						| cls.get_FLAG_SHOW_WHEN_LOCKED()
		;
		os::android::get_activity().getWindow().addFlags(flags);
	}
}



AMTRS_NAMESPACE_END
