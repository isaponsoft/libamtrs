/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__web__hpp
#define	__libamtrs__web__hpp
#include "amtrs.hpp"
AMTRS_NAMESPACE_BEGIN


AMTRS_NAMESPACE_END
#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
	#include "inc/web/web-win32.hpp"
#endif

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_ANDROID
	#include "inc/web/web-android.hpp"
#endif

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_IOS
	#include "inc/web/web-ios.hpp"
#endif

#endif
