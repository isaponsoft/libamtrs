/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__filesystem__enumfiles__hpp
#define	__libamtrs__filesystem__enumfiles__hpp
#include "../string/sstring.hpp"
#include "../typeutil/callback.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


struct	enumentry
{
	static constexpr unsigned int	fst_directory	= 1<<1;
	static constexpr unsigned int	fst_hidden		= 1<<2;
	static constexpr unsigned int	fst_execute		= 1<<3;
	static constexpr unsigned int	fst_symlink		= 1<<4;

	sstring			name;
	unsigned int	type;
};

/*
 * _dir
 *		調べたいディレクトリ
 *	_filter
 *		ファイル名のフィルタ(ワイルドカード)
 *		empty() の場合はフィルターしない
 */
AMTRS_API int enumfiles(text _dir, text _filter, typeutil::callback<bool(enumentry&)> _callback);


AMTRS_FILESYSTEM_NAMESPACE_END
#endif
