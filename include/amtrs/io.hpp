/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__hpp
#define	__libamtrs__io__hpp
#include "amtrs.hpp"
#include <ios>
#include <iosfwd>
#include <streambuf>

#define	AMTRS_IO_NAMESPACE_BEGIN		AMTRS_NAMESPACE_BEGIN namespace io {
#define	AMTRS_IO_NAMESPACE_END			} AMTRS_NAMESPACE_END
// dep
#define	AMTRS_IOSTREAM_NAMESPACE_BEGIN	AMTRS_IO_NAMESPACE_BEGIN
#define	AMTRS_IOSTREAM_NAMESPACE_END	AMTRS_IO_NAMESPACE_END
AMTRS_IO_NAMESPACE_BEGIN

enum class	openmode
{
	open			= 0<<0,			//!< ファイルを開く（存在しない場合はエラー）
	create			= 1<<0,			//!< ファイルを新規作成する（存在する場合はエラー）
	open_always		= 2<<0,			//!< ファイルを開く（存在しない場合は作成する）
	create_always	= 3<<0,			//!< ファイルを新規作成する（存在する場合は作成しなおす）

	use_read		= 1<<16,		//!< 読み込みを許可する
	use_write		= 1<<17			//!< 書き込みを許可する
};

static constexpr unsigned int	openmode_modemask	= 0x0f;

AMTRS_IO_NAMESPACE_END

#include ".inc/memory-view.hpp"
#include ".inc/typeutil-listener.hpp"

#include ".inc/io-streamif.hpp"
#include ".inc/io-streamif-iostream.hpp"
#include ".inc/io-streamif-stdlib.hpp"
#include ".inc/io-streamif-string_view.hpp"
#include ".inc/io-vstreamif.hpp"
#include ".inc/io-functions.hpp"
#include ".inc/iostream-bin.hpp"

#if		AMTRS_ZLIB_ENABLE
#include ".inc/io-zlib-stream_in.hpp"
#endif


// 以下廃止予定
#include ".inc/io-deserialize.hpp"
#include ".inc/io-limit.hpp"
#include ".inc/io-listener_stream.hpp"
#include ".inc/io-serialize.hpp"
#include ".inc/io-stream_in-fwd.hpp"
#include ".inc/io-stream_in-cstd.hpp"
#include ".inc/io-stream_in-stl.hpp"
#include ".inc/io-stream_in-view.hpp"




#endif
