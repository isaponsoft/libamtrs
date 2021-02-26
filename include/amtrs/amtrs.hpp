/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__amtrs__hpp
#define	__libamtrs__amtrs__hpp

/* namespace */
#ifndef	AMTRS_NAMESPACE
#define	AMTRS_NAMESPACE							amtrs
#endif

#define	AMTRS_NAMESPACE_BEGIN					namespace AMTRS_NAMESPACE {
#define	AMTRS_NAMESPACE_END						}
#define	AMTRS_STD_NAMESPACE_BEGIN				namespace std {
#define	AMTRS_STD_NAMESPACE_END					}

#define	AMTRS_IMPLEMENTS_BEGIN(_name)			namespace _name##_impl {
#define	AMTRS_IMPLEMENTS_END(_name)				}
#define	AMTRS_IMPLEMENTS(_name)					_name##_impl


#define	AMTRS_OS_NAMESPACE_BEGIN				AMTRS_NAMESPACE_BEGIN namespace os {
#define	AMTRS_OS_NAMESPACE_END					} AMTRS_NAMESPACE_END



/* c linkage */
#ifdef	__cplusplus
#define	AMTRS_EXTERN_C_BEGIN	extern "C" {
#define	AMTRS_EXTERN_C_END		}
#else
#define	AMTRS_EXTERN_C_BEGIN
#define	AMTRS_EXTERN_C_END
#endif


/******************************************************************************
 * Operating System
 * ----------------------------------------------------------------------------
 * #if AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
 * win32 codes.
 * #endif
 *****************************************************************************/
#define	AMTRS_PLATFORM_ANDROID		1							/* Google Android */
#define	AMTRS_PLATFORM_FREEBSD		2							/* FreeBSD */
#define	AMTRS_PLATFORM_IOS			3							/* Apple iOS */
#define	AMTRS_PLATFORM_LINUX		4							/* Linux */
#define	AMTRS_PLATFORM_DARWIN		5							/* Darwin */
#define	AMTRS_PLATFORM_MACOS		AMTRS_PLATFORM_DARWIN		/* Apple macOS */
#define	AMTRS_PLATFORM_OSX			AMTRS_PLATFORM_MACOS		/* Apple macOS */
#define	AMTRS_PLATFORM_RASBIAN		6							/* Raspberry PI Raspbian */
#define	AMTRS_PLATFORM_WIN32		7							/* Microsoft WIN32API */
#define	AMTRS_PLATFORM_WINRT		8							/* Microsoft WinRT(Windows Store) */


/* ==============================================================
 * Android
 */
#if 	defined(ANDROID) || defined(__ANDROID__) || defined(__ANDROID)

	//
	#define	AMTRS_CURRENT_PLATFORM		AMTRS_PLATFORM_ANDROID
	#define	AMTRS_CURRENT_PLATFORM_NAME	android
	#include <cstddef>


/* ==============================================================
 * Apple ... iOS & macOS
 */
#elif	defined(__APPLE__) && defined(__MACH__)

	// TargetConditionals が無いと判別がつかない部分があるので必ず
	// 取り込む
	#include <TargetConditionals.h>

	#if		(TARGET_OS_IPHONE == 1) || (TARGET_IPHONE_SIMULATOR == 1)
	#define	AMTRS_CURRENT_PLATFORM		AMTRS_PLATFORM_IOS
	#define	AMTRS_CURRENT_PLATFORM_NAME	ios
	#elif	(TARGET_OS_MAC == 1)
	#define	AMTRS_CURRENT_PLATFORM		AMTRS_PLATFORM_DARWIN
	#define	AMTRS_CURRENT_PLATFORM_NAME	darwin
	#endif


/* ==============================================================
 * Linux
 */
#elif defined(__linux__)

	#define	AMTRS_CURRENT_PLATFORM		AMTRS_PLATFORM_LINUX
	#define	AMTRS_CURRENT_PLATFORM_NAME	unixdrv


/* ==============================================================
 * FreeBSD
 */
#elif defined(__unix__) 

	#define	AMTRS_CURRENT_PLATFORM		AMTRS_PLATFORM_FREEBSD
	#define	AMTRS_CURRENT_PLATFORM_NAME	unixdrv


/* ==============================================================
 * Microsoft windows WINRT (depature)
 */
#elif	defined(__cplusplus_winrt)
// || defined(__cplusplus_cli)

	#define	AMTRS_CURRENT_PLATFORM		AMTRS_PLATFORM_WINRT
	#define	AMTRS_CURRENT_PLATFORM_NAME	winrt

/* ==============================================================
 * Microsoft windows WIN32
 */
#elif	defined(_WIN32) || defined(_WIN64)

	#define	AMTRS_CURRENT_PLATFORM		AMTRS_PLATFORM_WIN32
	#define	AMTRS_CURRENT_PLATFORM_NAME	win32

	/* Keep a include order. Include winwosk2 is fastest. */
	#define	NOMINMAX		1
	#include <winsock2.h>
	#include <windows.h>
	#include <windowsx.h>

#else

#endif



/*****************************************************************************
 * Include macro
 * --------------------------------------------------------------------------*/
#define	AMTRS_MAKE_PLATFORM_STRING__(_name)			#_name
#define	AMTRS_MAKE_PLATFORM_STRING_(_name)			AMTRS_MAKE_PLATFORM_STRING__(_name)
#define	AMTRS_CURRENT_PLATFORM_STRING				AMTRS_MAKE_PLATFORM_STRING_(AMTRS_CURRENT_PLATFORM_NAME)


#define	AMTRS_MAKE_INCLUDE_PLATFORM__(_filename)	#_filename
#define	AMTRS_MAKE_INCLUDE_PLATFORM_(_filename)		AMTRS_MAKE_INCLUDE_PLATFORM__(_filename)
#define	AMTRS_MAKE_INCLUDE_PLATFORM(_filename)		AMTRS_MAKE_INCLUDE_PLATFORM_(_filename-AMTRS_CURRENT_PLATFORM_NAME.hpp)


#define	AMTRS_PLATFORM_INCLUDE_(_filename)			<_filename>
#define	AMTRS_PLATFORM_INCLUDE(_filename)			AMTRS_PLATFORM_INCLUDE_(amtrs/.driver/AMTRS_CURRENT_PLATFORM_NAME-_filename)
#define	AMTRS_DRIVER_CUSTOM_INCLUDE(_filename)		AMTRS_PLATFORM_INCLUDE_(amtrs/.driver/_filename)
#define	AMTRS_DRIVER_INCLUDE(_filename)				AMTRS_DRIVER_CUSTOM_INCLUDE(AMTRS_CURRENT_PLATFORM_NAME-_filename)

/* basic header */
#include <stddef.h>


/* base library */
#include ".inc/amtrs-allocator.h"
#include ".inc/amtrs-stringview.h"


#ifdef	__cplusplus
AMTRS_NAMESPACE_BEGIN
AMTRS_NAMESPACE_END
AMTRS_OS_NAMESPACE_BEGIN
AMTRS_OS_NAMESPACE_END


// Basic headers
#include <algorithm>
#include <system_error>
#include ".inc/amtrs-bucket.hpp"
#include ".inc/amtrs-endian.hpp"
#include ".inc/amtrs-format.hpp"
#include ".inc/amtrs-logging.hpp"
#include ".inc/amtrs-range.hpp"
#include ".inc/amtrs-ref_object.hpp"

#endif
#endif
