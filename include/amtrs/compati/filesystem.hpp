/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__compati__filesystem__hpp
#define	__libamtrs__compati__filesystem__hpp


#if		(AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_ANDROID)

	#if	__NDK_MAJOR__ == 17

		// Android NDK 17 の場合は エミューションライブラリを使用する
		#define	AMTRS_USE_EXPERIMENTAL_FILESYSTEM	1
	#else
/*
	#if		__has_include(<filesystem>)
		// filesystem がある場合はそれを使う
		#define	AMTRS_USE_FILESYSTEM				1
		#include	<filesystem>
		AMTRS_FILESYSTEM_NAMESPACE_BEGIN
		namespace stdfs = std::filesystem;
		AMTRS_FILESYSTEM_NAMESPACE_END

	#elif	__has_include(<experimental/filesystem>)
		// experimental/filesystem がある場合はそちらを優先
		#define	AMTRS_USE_EXPERIMENTAL_FILESYSTEM	1
		#include	<experimental/filesystem>
		AMTRS_FILESYSTEM_NAMESPACE_BEGIN
		namespace stdfs = std::experimental::filesystem;
		AMTRS_FILESYSTEM_NAMESPACE_END

	#else
*/
	#endif


#elif	(AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_IOS)

#else

	#if		__has_include(<filesystem>)
		// filesystem がある場合はそれを使う
		#define	AMTRS_USE_FILESYSTEM				1
		#include	<filesystem>
		AMTRS_FILESYSTEM_NAMESPACE_BEGIN
		namespace stdfs = std::filesystem;
		AMTRS_FILESYSTEM_NAMESPACE_END
/*	#elif	__has_include(<experimental/filesystem>)
		// experimental/filesystem がある場合はそちらを優先
		#define	AMTRS_USE_EXPERIMENTAL_FILESYSTEM	1
		#include	<experimental/filesystem>
		AMTRS_FILESYSTEM_NAMESPACE_BEGIN
		namespace stdfs = std::experimental::filesystem;
		AMTRS_FILESYSTEM_NAMESPACE_END
*/	#endif
#endif


#if	!defined(AMTRS_USE_EXPERIMENTAL_FILESYSTEM) && !(AMTRS_USE_FILESYSTEM)

	// compatible library
	#include "filesystem/types.hpp"
	#include "filesystem/directory_entry.hpp"
	#include "filesystem/directory_iterator.hpp"
	#include "filesystem/filesystem_error.hpp"
	#include "filesystem/path.hpp"
//	#include "filesystem/default_aliases.hpp"

#endif	//__has_include(<filesystem>)


#endif
