/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__def__hpp
#define	__libamtrs__io__def__hpp
#include "../amtrs.hpp"
#define	AMTRS_IO_NAMESPACE_BEGIN	AMTRS_NAMESPACE_BEGIN namespace io {
#define	AMTRS_IO_NAMESPACE_END		} AMTRS_NAMESPACE_END
AMTRS_IO_NAMESPACE_BEGIN

enum class	openmode
{
	// 排他
	open			= 0<<0,			//!< ファイルを開く（存在しない場合はエラー）
	create			= 1<<0,			//!< ファイルを新規作成する（存在する場合はエラー）
	open_always		= 2<<0,			//!< ファイルを開く（存在しない場合は作成する）
	create_always	= 3<<0,			//!< ファイルを新規作成する（存在する場合は作成しなおす）

	use_read		= 1<<16,		//!< 読み込みを許可する
	use_write		= 1<<17			//!< 書き込みを許可する
};

static constexpr unsigned int	openmode_modemask	= 0x0f;

AMTRS_IO_NAMESPACE_END
#endif
