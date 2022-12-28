/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__base__platform__hpp
#define	__libamtrs__base__platform__hpp


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
	#define	AMTRS_CURRENT_PLATFORM			AMTRS_PLATFORM_ANDROID
	#define	AMTRS_CURRENT_PLATFORM_NAME		android
	#define	AMTRS_CURRENT_PLATFORM2_NAME	unxlk


/* ==============================================================
 * Apple ... iOS & macOS
 */
#elif	defined(__APPLE__) && defined(__MACH__)

	// TargetConditionals が無いと判別がつかない部分があるので必ず
	// 取り込む
	#include <TargetConditionals.h>

	#if		(TARGET_OS_IPHONE == 1) || (TARGET_IPHONE_SIMULATOR == 1)
	#define	AMTRS_CURRENT_PLATFORM			AMTRS_PLATFORM_IOS
	#define	AMTRS_CURRENT_PLATFORM_NAME		ios
	#elif	(TARGET_OS_MAC == 1)
	#define	AMTRS_CURRENT_PLATFORM			AMTRS_PLATFORM_DARWIN
	#define	AMTRS_CURRENT_PLATFORM_NAME		darwin
	#endif
	#define	AMTRS_CURRENT_PLATFORM2_NAME	unxlk


/* ==============================================================
 * Linux
 */
#elif defined(__linux__)

	#define	AMTRS_CURRENT_PLATFORM			AMTRS_PLATFORM_LINUX
	#define	AMTRS_CURRENT_PLATFORM_NAME		unxlk
	#define	AMTRS_CURRENT_PLATFORM2_NAME	linuxdrv


/* ==============================================================
 * FreeBSD
 */
#elif defined(__unix__) 

	#define	AMTRS_CURRENT_PLATFORM			AMTRS_PLATFORM_FREEBSD
	#define	AMTRS_CURRENT_PLATFORM_NAME		unxlk
	#define	AMTRS_CURRENT_PLATFORM2_NAME	freebsd


/* ==============================================================
 * Microsoft windows WINRT (depature)
 */
#elif	defined(__cplusplus_winrt)
// || defined(__cplusplus_cli)

	#define	AMTRS_CURRENT_PLATFORM			AMTRS_PLATFORM_WINRT
	#define	AMTRS_CURRENT_PLATFORM_NAME		winrt

/* ==============================================================
 * Microsoft windows WIN32
 */
#elif	defined(_WIN32) || defined(_WIN64)

	#define	AMTRS_CURRENT_PLATFORM			AMTRS_PLATFORM_WIN32
	#define	AMTRS_CURRENT_PLATFORM_NAME		win32

	#if		!defined __WIN32__
	#define	__WIN32__
	#endif

	/* Keep a include order. Include winwosk2 is fastest. */
	#ifndef	NOMINMAX
	#define	NOMINMAX		1
	#endif
	#include <winsock2.h>
	#include <windows.h>
	#include <windowsx.h>

#else

#endif



/*****************************************************************************
 * Include macro
 * --------------------------------------------------------------------------*/
#define	AMTRS_MAKE_PLATFORM_STRING(_name)			AMTRS_MAKE_PLATFORM_STRING__(_name)
#define	AMTRS_MAKE_PLATFORM_STRING__(_name)			#_name
#define	AMTRS_MAKE_PLATFORM_STRING_(_name)			AMTRS_MAKE_PLATFORM_STRING__(_name)
#define	AMTRS_CURRENT_PLATFORM_STRING				AMTRS_MAKE_PLATFORM_STRING_(AMTRS_CURRENT_PLATFORM_NAME)

#define	AMTRS_MAKE_INCLUDE_PLATFORM__(_filename)	#_filename
#define	AMTRS_MAKE_INCLUDE_PLATFORM_(_filename)		AMTRS_MAKE_INCLUDE_PLATFORM__(_filename)
#define	AMTRS_MAKE_INCLUDE_PLATFORM(_filename)		AMTRS_MAKE_INCLUDE_PLATFORM_(_filename-AMTRS_CURRENT_PLATFORM_NAME.hpp)


#define	AMTRS_PLATFORM_INCLUDE_(_filename)			<_filename>
#define	AMTRS_PLATFORM_INCLUDE(_filename)			AMTRS_PLATFORM_INCLUDE_(amtrs/.driver/AMTRS_CURRENT_PLATFORM_NAME/AMTRS_CURRENT_PLATFORM_NAME-_filename)
#define	AMTRS_DRIVER_CUSTOM_INCLUDE(_filename)		AMTRS_PLATFORM_INCLUDE_(amtrs/.driver/_filename)
#define	AMTRS_DRIVER_INCLUDE(_filename)				AMTRS_DRIVER_CUSTOM_INCLUDE(AMTRS_CURRENT_PLATFORM_NAME/AMTRS_CURRENT_PLATFORM_NAME-_filename)


// use. libamtrs source code only.
#define	AMTRS_PLATFORM_SUBCODE(_suffix)				AMTRS_PLATFORM_STRING(_suffix.cxx)
#define	AMTRS_PLATFORM_STRING(_suffix)				AMTRS_PLATFORM_STRING_(AMTRS_CURRENT_PLATFORM_NAME,_suffix)
#define	AMTRS_PLATFORM_STRING_(_a,_b)				AMTRS_PLATFORM_STRING__(_a,_b)
#define	AMTRS_PLATFORM_STRING__(_a,_b)				AMTRS_MAKE_PLATFORM_STRING__(drv-_a/_b)


#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#	define	AMTRS_FORCE_EXPORT	__declspec(dllexport)
#	define	AMTRS_FORCE_IMPORT	__declspec(dllimport)
#	if		defined(AMTRS_EXPORT_DLL)
#	define	AMTRS_API	AMTRS_FORCE_EXPORT
#	elif	defined(AMTRS_IMPORT_DLL)
#	define	AMTRS_API	AMTRS_FORCE_IMPORT
#	else
#	define	AMTRS_API
#	endif
#else//	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#	define	AMTRS_FORCE_EXPORT
#	define	AMTRS_FORCE_IMPORT
#	define	AMTRS_API
#endif


/* c linkage */
#ifdef	__cplusplus
#define	AMTRS_EXTERN_C_BEGIN	extern "C" {
#define	AMTRS_EXTERN_C_END		}
#else
#define	AMTRS_EXTERN_C_BEGIN
#define	AMTRS_EXTERN_C_END
#endif


/* basic header */
#ifdef	__cplusplus
#include <cstddef>
#include <cstdint>
#include <utility>
#else
#include <stddef.h>
#endif

#if		__cplusplus	>= 201703L
#define	AMTRS_CPP_VER	17
#define	amtrs_constexpr	constexpr
#else
#define	AMTRS_CPP_VER	14
#define	amtrs_constexpr
#endif

#endif
