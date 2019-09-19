/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__filesystem__types__hpp
#define __libamtrs__filesystem__types__hpp
#include "def.hpp"


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
	// どれもないので互換ライブラリ（エミュレーション）
	#include "stdfs/file_status.hpp"
	#include "stdfs/types.hpp"
	#include "stdfs/path.hpp"
	#include "stdfs/function-posix.hpp"
	#include "stdfs/directory_entry.hpp"
	#include "stdfs/directory_iterator.hpp"
	#include "stdfs/filesystem_error.hpp"
/*	#include "compati/filesystem.hpp"
	#include "compati/path.hpp"
	#include "compati/path_util.hpp"
	#include "compati/util.hpp"
	#include "compati/default_aliases.hpp"
*/
#endif	//__has_include(<filesystem>)
AMTRS_FILESYSTEM_NAMESPACE_BEGIN
using namespace stdfs;

// std::filesystem があれば、std::filesystem エイリアスを設定し、
// なければ、std::filesystem の互換ライブラリのエイリアスを使用します。
// アプリケーションからは std::filesystem の代わりに amtrs::filesystem を使うと
// 自動的に適切な filesystem が使用されます。
/*
using	file_type	= stdfs::file_type;
using	perms		= stdfs::perms;
using	file_status	= stdfs::file_status;
using	path		= stdfs::path;
*/

AMTRS_FILESYSTEM_NAMESPACE_END
#endif
