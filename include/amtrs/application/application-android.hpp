/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__application__application_android__hpp
#define	__libamtrs__application__application_android__hpp
#include "../os/android/android_app.hpp"
#include "../os/android/amtrs_activity.hpp"
#include "../java/java/io/File.hpp"
#include "../java/android/app/Activity.hpp"
#include "../java/android/content/Context.hpp"
#include "../java/android/content/pm/ApplicationInfo.hpp"
#include "../java/android/content/Intent.hpp"
#include "../java/android/os/Environment.hpp"
#include "../java/android/os/storage/StorageManager.hpp"
#include "../java/android/os/storage/StorageVolume.hpp"
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
