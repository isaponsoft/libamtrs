/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__hpp
#define	__libamtrs__io__hpp
#include "amtrs.hpp"
#include <ios>
#include <iosfwd>
#include <streambuf>

// dep
#define	AMTRS_IOSTREAM_NAMESPACE_BEGIN	AMTRS_IO_NAMESPACE_BEGIN
#define	AMTRS_IOSTREAM_NAMESPACE_END	AMTRS_IO_NAMESPACE_END
AMTRS_IO_NAMESPACE_BEGIN

enum class	openmode
{
	open			= 0,			//!< ファイルを開く（存在しない場合はエラー）
	create			= 1,			//!< ファイルを新規作成する（存在する場合はエラー）
	open_always		= 2,			//!< ファイルを開く（存在しない場合は作成する）
	create_always	= 3,			//!< ファイルを新規作成する（存在する場合は作成しなおす）
};

enum class	accessmode
{
	read_only		= 0,
	write_only		= 1,
	read_write		= 2,
};


using	iostate	= std::ios_base::iostate;
using	seekdir	= std::ios_base::seekdir;
static constexpr seekdir	seekdir_beg			= std::ios_base::beg;
static constexpr seekdir	seekdir_cur			= std::ios_base::cur;
static constexpr seekdir	seekdir_end			= std::ios_base::end;


static constexpr unsigned int	openmode_modemask	= 0x0f;


inline seekdir to_seekdir(amtrs_seekdir _ctype) noexcept
{
	return	_ctype == amtrs_seekdir_cur ? seekdir_cur
		:	_ctype == amtrs_seekdir_end ? seekdir_end
		:                                 seekdir_beg;
}

AMTRS_IO_NAMESPACE_END



#include ".inc/memory-view.hpp"
#include ".inc/typeutil-listener.hpp"
#include ".inc-io/io-functions.hpp"
#include ".inc-io/io-filedescriptor.hpp"
#include ".inc-io/io-stream.hpp"
#include ".inc-io/io-streamif.hpp"
#include ".inc-io/io-streamif-iostream.hpp"
#include ".inc-io/io-streamif-stdlib.hpp"
#include ".inc-io/io-streamif-string_view.hpp"
#include ".inc-io/io-omemory_stream.hpp"
#include ".inc-io/io-vstreamif.hpp"
#include ".inc-io/iostream-bin.hpp"
//#include ".inc-io/io-zlib-stream_in.hpp"

// 以下廃止予定
#include ".inc-io/io-deserialize.hpp"
#include ".inc-io/io-limit.hpp"
#include ".inc-io/io-listener_stream.hpp"
#include ".inc-io/io-serialize.hpp"
#include ".inc-io/io-stream_in-fwd.hpp"
#include ".inc-io/io-stream_in-cstd.hpp"
#include ".inc-io/io-stream_in-stl.hpp"
#include ".inc-io/io-stream_in-view.hpp"




#endif
