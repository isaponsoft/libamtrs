/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__main__hpp
#define	__libamtrs__main__hpp
#include "amtrs.hpp"
#include "application.hpp"

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
	#include "inc/main/main-win32.hpp"
#endif

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WINRT
	#include "inc/main/main-winrt.hpp"
#endif

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_ANDROID
	#include "inc/main/main-android.hpp"
#endif

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_IOS
	#include "inc/main/main-ios.hpp"
#endif

#endif
