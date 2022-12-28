/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <dlfcn.h>
#include <algorithm>
#include <amtrs/system/dynamic_library.hpp>
AMTRS_SYSTEM_NAMESPACE_BEGIN

dynamic_library::dynamic_library(text _filename, bool _add_ext)
{
	sstring	libname(_filename);
	if (_add_ext)
	{
		libname.append(".so");
	}

	mHandle	= dlopen(libname.c_str(), RTLD_LAZY);
}

void dynamic_library::clear()
{
	if (mHandle)
	{
		dlclose(mHandle);
		mHandle	= nullptr;
	}
}

void* dynamic_library::get_address(text _symbol)
{
	sstring	name(_symbol);
	return	dlsym(mHandle, name.c_str());
}

AMTRS_SYSTEM_NAMESPACE_END
