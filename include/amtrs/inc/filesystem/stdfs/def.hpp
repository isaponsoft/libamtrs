/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__filesystem__stdfs__def__hpp
#define	__libamtrs__filesystem__stdfs__def__hpp
// std::filesystem が存在しない場合の代理ライブラリ
#include "../def.hpp"
#define	AMTRS_FILESYSTEM_STDFS_NAMESPACE		stdfs
#define	AMTRS_FILESYSTEM_STDFS_NAMESPACE_BEGIN	AMTRS_FILESYSTEM_NAMESPACE_BEGIN namespace AMTRS_FILESYSTEM_STDFS_NAMESPACE {
#define	AMTRS_FILESYSTEM_STDFS_NAMESPACE_END	} AMTRS_FILESYSTEM_NAMESPACE_END
#endif
