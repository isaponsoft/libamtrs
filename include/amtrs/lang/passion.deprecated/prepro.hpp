/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__prepro__hpp
#define	__libamtrs__passion__prepro__hpp
#include "vm.hpp"
#include "lang-lexer.hxx"
PSNVM_PREPRO_NAMESPACE_BEGIN

struct	declare
{
	sstring	filename;
	int		line;
	sstring	code;
};

struct	group
{
	sstring					identify;
	simplearray<declare>	decls;	
};

struct	prepro : errorhandler
{
	// 削除した行を置き換えた後の文字列(デフォルトは "\n")
	text					removed_line;
	text					removed_line_begin;
	text					removed_line_end;
	simplearray<group>		defines;
	simplearray<sstring>	paths;

	prepro(text _removed_line = "\n")
		: removed_line(_removed_line)
	{}

	group* find_macro_group(text _name)
	{
		for (auto& g : defines)
		{
			if (g.identify == _name)
			{
				return	&g;
			}
		}
		return	nullptr;
	}
};


PSNVM_API sstring process(prepro& _pp, text _code, text _filename);


PSNVM_PREPRO_NAMESPACE_END
#endif
