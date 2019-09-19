#ifndef	__libamtrs__os__def__hpp
#define	__libamtrs__os__def__hpp
#include "../amtrs.hpp"
#define	AMTRS_OS_NAMESPACE_BEGIN	AMTRS_NAMESPACE_BEGIN namespace os {
#define	AMTRS_OS_NAMESPACE_END		} AMTRS_NAMESPACE_END
AMTRS_OS_NAMESPACE_BEGIN
AMTRS_OS_NAMESPACE_END


// ****************************************************************************
//! Operating System
// ----------------------------------------------------------------------------
//! #if AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
//! win32 codes.
//! #endif
// ****************************************************************************
#define	AMTRS_PLATFORM_ANDROID		1							// Google Android
#define	AMTRS_PLATFORM_FREEBSD		2							// FreeBSD
#define	AMTRS_PLATFORM_IOS			3							// Apple iOS
#define	AMTRS_PLATFORM_LINUX		4							// Linux
#define	AMTRS_PLATFORM_MACOS		5							// Apple macOS
#define	AMTRS_PLATFORM_OSX			AMTRS_PLATFORM_MACOS		// Apple macOS
#define	AMTRS_PLATFORM_RASBIAN		6							// Raspberry PI Raspbian
#define	AMTRS_PLATFORM_WIN32		7							// Microsoft WIN32API
#define	AMTRS_PLATFORM_WINRT		8							// Microsoft WinRT



// ==============================================================
// Android
#if 	defined(ANDROID) || defined(__ANDROID__) || defined(__ANDROID)

	//
	#define	AMTRS_CURRENT_PLATFORM		AMTRS_PLATFORM_ANDROID
	#define	AMTRS_CURRENT_PLATFORM_NAME	android
	#include <cstddef>


// ==============================================================
// Apple ... iOS と macOS の選定
#elif	defined(__APPLE__) && defined(__MACH__)

	// TargetConditionals が無いと判別がつかない部分があるので必ず
	// 取り込む
	#include <TargetConditionals.h>

	#if		(TARGET_OS_IPHONE == 1) || (TARGET_IPHONE_SIMULATOR == 1)
	#define	AMTRS_CURRENT_PLATFORM		AMTRS_PLATFORM_IOS
	#define	AMTRS_CURRENT_PLATFORM_NAME	ios
	#elif	(TARGET_OS_MAC == 1)
	#define	AMTRS_CURRENT_PLATFORM		AMTRS_PLATFORM_MACOS
	#define	AMTRS_CURRENT_PLATFORM_NAME	osx
	#endif

// ==============================================================
// Linux
#elif defined(__linux__)

	#define	AMTRS_CURRENT_PLATFORM		AMTRS_PLATFORM_LINUX
	#define	AMTRS_CURRENT_PLATFORM_NAME	linux

// ==============================================================
// FreeBSD
#elif defined(__unix__) 

	#define	AMTRS_CURRENT_PLATFORM		AMTRS_PLATFORM_FREEBSD
	#define	AMTRS_CURRENT_PLATFORM_NAME	freebsd

// ==============================================================
// Microsoft ... WinRT
#elif	defined(__cplusplus_winrt) || defined(__cplusplus_cli)

	#define	AMTRS_CURRENT_PLATFORM		AMTRS_PLATFORM_WINRT
	#define	AMTRS_CURRENT_PLATFORM_NAME	winrt
static_error(false, "Unkown platform");
// ==============================================================
// Microsoft ... Win32
#elif	defined(_WIN32) || defined(_WIN64)

	#define	AMTRS_CURRENT_PLATFORM		AMTRS_PLATFORM_WIN32
	#define	AMTRS_CURRENT_PLATFORM_NAME	win32

#else

static_error(false, "Unkown platform");

#endif


#define	AMTRS_MAKE_PLATFORM_STRING__(_name)			#_name
#define	AMTRS_MAKE_PLATFORM_STRING_(_name)			AMTRS_MAKE_PLATFORM_STRING__(_name)
#define	AMTRS_CURRENT_PLATFORM_STRING				AMTRS_MAKE_PLATFORM_STRING_(AMTRS_CURRENT_PLATFORM_NAME)


#define	AMTRS_MAKE_INCLUDE_PLATFORM__(_filename)	#_filename
#define	AMTRS_MAKE_INCLUDE_PLATFORM_(_filename)		AMTRS_MAKE_INCLUDE_PLATFORM__(_filename)
#define	AMTRS_MAKE_INCLUDE_PLATFORM(_filename)		AMTRS_MAKE_INCLUDE_PLATFORM_(_filename-AMTRS_CURRENT_PLATFORM_NAME.hpp)


#endif
