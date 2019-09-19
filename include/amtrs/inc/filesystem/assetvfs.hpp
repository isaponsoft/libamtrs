/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__filesystem__assetvfs__hpp
#define __libamtrs__filesystem__assetvfs__hpp
#include "def.hpp"
#include "types.hpp"
#include "vfs.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN
AMTRS_FILESYSTEM_NAMESPACE_END

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_ANDROID
#include "assetvfs-android.hpp"
#endif

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_IOS
#include "assetvfs-ios.hpp"
#endif

#endif
